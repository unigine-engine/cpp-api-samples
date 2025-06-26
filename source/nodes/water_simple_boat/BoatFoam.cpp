#include "BoatFoam.h"
#include <UnigineConsole.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(BoatFoam);

using namespace Unigine;
using namespace Math;


void BoatFoam::init()
{
	std::function<void(const NodePtr&)> collect = [this, &collect](const NodePtr &in_node)
	{
		if (!in_node)
			return;
		ObjectParticlesPtr p = checked_ptr_cast<ObjectParticles>(in_node);
		if (p)
		{
			auto & particles = foam_particles.append();
			particles.particles = p;
			particles.life_mean = p->getLifeMean();
			particles.life_spread = p->getLifeSpread();
			particles.spaw_rate = p->getSpawnRate();
			particles.velocity_min = p->getVelocityOverTimeModifier()->getConstantMin();
			particles.velocity_max = p->getVelocityOverTimeModifier()->getConstantMax();
		}

		NodeReferencePtr nr = checked_ptr_cast<NodeReference>(in_node);
		if (nr)
		{
			collect(nr->getReference());
		}

		for (int i = 0 ; i < in_node->getNumChildren(); i++)
		{
			collect(in_node->getChild(i));
		}
	};

	foam = checked_ptr_cast<DecalOrtho>(decal_foam.get());
	collect(foam);
}

void BoatFoam::update()
{
	float ifps = Game::getIFps();
	Vec3 p = node->getWorldPosition();
	Vec3 velocity = (p - last_position) / ifps;
	velocity = clamp(velocity, Vec3(-100, -100, 0), Vec3(100,100,0));
	velocity_scalar = Unigine::Math::lerp(velocity_scalar, float(velocity.length()), ifps);
	last_position = p;

	update_foam_decal_size();
	update_particles_params();
}

void BoatFoam::shutdown()
{

}

void BoatFoam::update_foam_decal_size()
{
	if (foam_particles.size() == 0) {
		return;
	}

	Vec3 pos = node->getWorldPosition();
	WorldBoundBox decal_bound_box(pos, pos);
	for (const auto & it : foam_particles)
	{
		decal_bound_box.expand(it.particles->getWorldBoundBoxParticles());
	}

	Vec3 min = decal_bound_box.minimum - Vec3(decal_padding);
	Vec3 max = decal_bound_box.maximum + Vec3(decal_padding);
	foam->setWidth((float)max.x - (float)min.x);
	foam->setHeight((float)max.y - (float)min.y);

	Vec3 center = (max + min) * 0.5f;
	center.z = pos.z + foam->getRadius() * 0.5f;

	Mat4 old_transform = foam->getTransform();
	foam->setWorldPosition(center);
	foam->setWorldRotation(quat_identity, true);
	Mat4 new_transform = foam->getTransform();

	Mat4 offset = inverse(new_transform) * old_transform;
	for(const auto & it : foam_particles)
	{
		it.particles->setTransform(offset * it.particles->getTransform());
	}
}

void BoatFoam::update_particles_params()
{
	float spawn_scale = clamp(velocity_scalar / max_ship_velocity_for_spawn_scale, 0.0f, 1.0f);
	float life_scale = clamp(velocity_scalar / max_ship_velocity_for_life_scale, 0.0f, 1.0f);
	float velocity_scale = clamp(velocity_scalar / max_ship_velocity_for_velocity_scale, 0.0f, 1.0f);

	float global_lifetime_param = Console::getFloat("global_lifetime");

	for(const auto & it : foam_particles) {

		it.particles->setSpawnRate(it.spaw_rate * spawn_scale);
		it.particles->setLife(it.life_mean * life_scale * global_lifetime_param, it.life_spread * life_scale);

		auto velocity_modifier = it.particles->getVelocityOverTimeModifier();
		velocity_modifier->setConstantMin(it.velocity_min * velocity_scale);
		velocity_modifier->setConstantMax(it.velocity_max * velocity_scale);
	}
}

