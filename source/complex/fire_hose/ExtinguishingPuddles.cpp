#include "ExtinguishingPuddles.h"
#include <UnigineWorld.h>
#include <UnigineGame.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(ExtinguishingPuddles);

void ExtinguishingPuddles::init()
{
	emitter = checked_ptr_cast<ObjectParticles>(node);
	if (!emitter)
	{
		Log::error("%s: ExtinguishingPuddles component can only be assigned to ObjectParticles\n", __FUNCTION__);
		return;
	}

	// Creating a temporary decal for cloning
	auto tmp = World::loadNode(decal_ref.get());
	
	if (tmp)
	{
		tmp->setEnabled(false);

		decal_prefab = checked_ptr_cast<Decal>(tmp);
		if (decal_prefab)
		{
			decal_albedo_id = decal_prefab->getMaterial()->findParameter("albedo");
		}
		else
		{
			Log::error("%s: can't load %s decal\n", __FUNCTION__, decal_ref.get());
			return;
		}
	} 
	else
	{
		Log::error("%s: can't load %s NodeReference \n", __FUNCTION__, decal_ref.get());
		return;
	}
	spark_emitter = checked_ptr_cast<ObjectParticles>(spark_emitter_node.get());

	cos_min_spawn_angle = Math::cos(min_spawn_angle * Math::Consts::DEG2RAD);
	cos_max_spawn_angle = Math::cos(max_spawn_angle * Math::Consts::DEG2RAD);

	emitter->setFPSInvisible(30);
	emitter->setUpdateDistanceLimit(1e+4f);

	if (dead_time <= 0.0f)
		dead_time = 0.01f;
}

void ExtinguishingPuddles::update()
{
	update_decal_traces();

	if (!emitter || !emitter->isEmitterEnabled() || !decal_prefab)
			return;

	if (emitter->getNumContacts() == 0)
		return;

	// Creation of decals at the point of contact between particles and the surface.
	for (int i = 0; i < emitter->getNumContacts(); i++)
	{
		ObjectPtr contact_obj = emitter->getContactObject(i);

		Math::Vec3 contact_point = emitter->getContactPoint(i);
		Math::vec3 contact_normal = emitter->getContactNormal(i);
		Math::vec3 contact_velocity = emitter->getContactVelocity(i);

		float cos_contact_angle = contact_normal.z;
		if (cos_min_spawn_angle < cos_contact_angle || cos_max_spawn_angle > cos_contact_angle) // cos(max) < cos(a) < cos(min)
			continue;

		NodePtr decal_node = decal_prefab->clone();
		decal_node->setEnabled(true);

		auto & data = decal_traces.append();
		data.node = decal_node;
		data.material = checked_ptr_cast<Decal>(decal_node)->getMaterialInherit();
		data.life = 1.0f; // 100%
		data.life_time = life_time;
		data.gravity_drag = Math::vec3_zero;
		data.ground_drag = Math::vec3_zero;
		data.ground_velocity = max(0.0f,Game::getRandomFloat(ground_speed - ground_speed_spread, ground_speed + ground_speed_spread));
		data.ground_damping = max(0.0f,Game::getRandomFloat(ground_damping - ground_damping_spread, ground_damping + ground_damping_spread));
		data.scale_velocity = grow_speed;

		bool on_ground = Math::abs(contact_normal.z - 1.0f) < Math::Consts::EPS;
		if (!on_ground)
		{
			data.gravity_drag = normalize(Math::cross(Math::cross(contact_normal, Math::vec3_down), contact_normal));
			decal_node->setWorldDirection(-contact_normal, Math::vec3_up);
		}
		else
		{
			data.ground_drag = normalize(Math::cross(Math::cross(contact_normal, contact_velocity), contact_normal));
			data.ground_velocity *= Math::saturate(dot(data.ground_drag, normalize(contact_velocity)));
		}

		// Makes the particles of the visual emitter behave according to the behavior of the particles of this emitter,
		// which are used to process the interaction.
		if (spark_emitter)
		{
			Math::vec3 direction = contact_velocity;
			direction.z = 0.0f;
			direction.normalize();

			auto dm = spark_emitter->getDirectionOverTimeModifier();
			dm->setMode(ParticleModifier::MODE::MODE_CONSTANT);
			dm->setConstant(direction);

			auto vm = spark_emitter->getVelocityOverTimeModifier();
			vm->setMode(ParticleModifier::MODE::MODE_RANDOM_BETWEEN_TWO_CONSTANTS);
			vm->setConstantMin(on_ground ? 0.8f : 0);
			vm->setConstantMax(on_ground ? 1.2f : 0);
			spark_emitter->setDirection((on_ground) ? direction : Math::vec3_zero, Math::vec3_up, AXIS_Y);
			auto lm = spark_emitter->getLinearDampingOverTimeModifier();
			lm->setMode(ParticleModifier::MODE_CONSTANT);
			lm->setConstant(spark_linear_damping);
		}

		// Set decal transform
		decal_node->setWorldPosition(contact_point + Math::Vec3(contact_normal * decal_node->getBoundSphere().radius * 1.0f));
	    decal_node->setWorldRotation(Math::quat(contact_normal, Game::getRandomFloat(0.0f, 360.0f)) * decal_node->getWorldRotation());
		decal_node->setWorldScale(Math::vec3_one * Game::getRandomFloat(min_spawn_scale, max_spawn_scale));
	}
}

void ExtinguishingPuddles::post_update()
{
	if (decal_albedo_id == -1)
			return;
	// Changing the material's transparency
	for (auto & data : decal_traces)
	{
		Math::vec4 albedo = data.material->getParameterFloat4(decal_albedo_id);
		albedo.w *= data.life;
		data.material->setParameterFloat4(decal_albedo_id, albedo);
	}
}

void ExtinguishingPuddles::shutdown()
{
	for (auto & it : decal_traces)
		it.clear();
	decal_traces.clear();
}

void ExtinguishingPuddles::update_decal_traces()
{
	float dt = Game::getIFps();

	// Reduces lifetime of excess decals
	if (decal_traces.size() > max_count)
	{
		for (int i = 0; i < decal_traces.size() - max_count; i++)
		{
			if (decal_traces[i].life_time > dead_time)
				decal_traces[i].life_time = dead_time;
		}
	}

	// Decal state update 
	for (int i = decal_traces.size() - 1; i >= 0; i--)
	{
		DecalTraceData &data = decal_traces[i];
		data.life -= dt / data.life_time;

		if (data.life <= 0.0f)
		{
			auto d = decal_traces.takeAt(i);
			d.clear();
			continue;
		}

		Math::Vec3 position = data.node->getWorldPosition();
		Math::vec3 scale = data.node->getScale();

		position += Math::Vec3(data.gravity_drag * dt * vertical_speed);
		position += Math::Vec3(data.ground_drag * dt * data.ground_velocity);

		data.ground_velocity = Math::max(0.0f, data.ground_velocity - data.ground_damping * dt);

		scale += Math::vec3_one * data.scale_velocity * dt;
		data.scale_velocity = Math::max(0.0f, data.scale_velocity - grow_damping * dt);

		data.node->setScale(scale);
		data.node->setWorldPosition(position);
	}
}
