#include "SlingRope.h"

#include <UnigineGame.h>
#include <UniginePhysics.h>
#include <UnigineVisualizer.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(SlingRope)

void SlingRope::setLength(float len)
{
	// minimum of two segments are required for joints to work properly
	len = Math::max(2.0f * segment_length, len);
	if (len == rope_length)
		return;

	rope_length = len;

	// get integer and fractional part on number of segments
	float segments = rope_length / segment_length;
	int new_num_segments = ftoi(ceilf(segments));
	segments_offset = frac(segments);

	// update rope anchor position to change length smoothly
	if (rope_obj)
	{
		Vec3 anchor_pos = anchor_obj->getWorldPosition();
		if (segments_offset != 0.0f)
			anchor_pos += Vec3_up * segment_length * (1.0f - segments_offset);

		anchor_body->setTransform(translate(anchor_pos));
	}

	// add new segments
	if (new_num_segments != num_segments)
		change_num_segments(new_num_segments - num_segments);
}

void SlingRope::attachBag()
{
	if (bag_attached)
		return;

	bag_node->setWorldTransform(translate(rope_body->getParticlePosition(0)
		- (bag_anchor->getWorldPosition() - bag_body->getPosition())));
	rope_body->setParticlePosition(0, bag_anchor->getWorldPosition());

	bag_attached = create_bag_joint();
}

void SlingRope::detachBag()
{
	if (bag_attached && bag_joint)
	{
		if (rope_body)
			rope_body->removeJoint(bag_joint);
		bag_joint.deleteLater();
		bag_attached = false;
	}
}

void SlingRope::setBagMass(float mass)
{
	if (bag_body)
	{
		if (bag_body->isShapeBased() && bag_body->getNumShapes() > 0)
			bag_body->getShape(0)->setMass(mass);
		else
			bag_body->setMass(mass);
	}
}

float SlingRope::getBagMass() const
{
	if (bag_body)
		return bag_body->getMass();

	return 0.0;
}

void SlingRope::setTensionCompensationEnabled(bool enable)
{
	tension_enabled = enable;
	if (!enable)
		rope_body->setMass(mass);
}

void SlingRope::setDebugEnabled(bool enable)
{
	debug_enabled = enable;
	Visualizer::setEnabled(debug_enabled);
}

void SlingRope::init()
{
	// init anchor object and dummy body where rope will be attached to crane
	anchor_obj = checked_ptr_cast<ObjectDummy>(node);
	if (!anchor_obj)
	{
		Log::error("%s(): SlingRope node must be ObjectDummy!\n", __FUNCTION__);
		return;
	}
	anchor_body = checked_ptr_cast<BodyDummy>(anchor_obj->getBody());
	if (!anchor_body)
		anchor_body = BodyDummy::create(anchor_obj);

	// create ObjectMeshDynamic responsible for rope visualization
	rope_obj = ObjectMeshDynamic::create();
	rope_obj->setParent(anchor_obj);
	rope_obj->setMaterial(rope_mat.get(), "*");

	// init bag body
	if (bag_node.nullCheck())
	{
		Log::error("%s(): bag node must be not null!\n", __FUNCTION__);
		return;
	}

	bag_body = bag_node->getObjectBodyRigid();
	if (!bag_body)
	{
		Log::error("%s(): node \"%s\" must have BodyRigid!\n", __FUNCTION__, bag_node->getName());
		return;
	}
	bag_body->setFreezable(false);
	bag_body->setFrozen(false);

	// init bag anchor node where rope will be attached to bag
	if (bag_anchor_node.nullCheck())
		bag_anchor = bag_node;
	else
		bag_anchor = bag_anchor_node.get();

	// create rope from crane anchor point to bag anchor point
	setLength(static_cast<float>(
		length(bag_anchor->getWorldPosition() - anchor_obj->getWorldPosition())));

	min_tension_cos = Math::cos(min_tension_angle * Consts::DEG2RAD);
}

void SlingRope::update()
{
	// TODO remove joint visualizer
	if (bag_joint)
		bag_joint->renderVisualizer(vec4(1.f, 1.f, 0.f, 1.f));

	collect_particles_data();
	if (debug_enabled)
		show_debug();
	else
		refresh_mesh();

	if (tension_enabled)
	{
		// adjust rope mass to compensate tension
		if (check_tension())
			rope_body->setMass(tension_mass_multiplier * bag_body->getMass());
		else
			rope_body->setMass(mass);
	}
}

void SlingRope::shutdown()
{
	Visualizer::setEnabled(false);

	// remove all joints
	if (anchor_joint)
	{
		if (rope_body)
			rope_body->removeJoint(anchor_joint);
		anchor_joint.deleteLater();
	}

	if (bag_joint)
	{
		if (rope_body)
			rope_body->removeJoint(bag_joint);
		bag_joint.deleteLater();
	}

	// remove rope
	rope_body.deleteLater();
	base_rope_obj.deleteLater();
	rope_obj.deleteLater();

	particles_position.clear();
	particles_velocity.clear();
}

bool SlingRope::check_tension()
{
	if (!bag_attached || !rope_body || !bag_body)
		return false;

	if (rope_length < min_tension_length)
		return true;

	if (rope_body->getNumContacts())
		return false;

	for (int i = 0; i < particles_position.size() - 2; i++)
	{
		Vec3 p0 = particles_position[i];
		Vec3 p1 = particles_position[i + 1];
		Vec3 p2 = particles_position[i + 2];

		float cos = static_cast<float>(dot((p1 - p0).normalize(), (p2 - p1).normalize()));
		if (cos < min_tension_cos)
			return false;
	}
	return true;
}

void SlingRope::change_num_segments(int num)
{
	// save rope info
	collect_particles_data();

	float current_mass = mass;
	if (rope_body)
	{
		current_mass = rope_body->getMass();
		rope_body.deleteLater();
	}

	// remove anchor joint
	if (anchor_joint)
		anchor_joint.deleteLater();

	// remove bag joint
	if (bag_joint)
		bag_joint.deleteLater();

	num_segments += num;
	if (num_segments < 1)
		num_segments = 1;

	// create new base rope object for BodyRope
	update_internal_mesh();

	// create rope body
	rope_body = BodyRope::create(base_rope_obj);
	rope_body->setRadius(radius);
	rope_body->setMass(current_mass);
	rope_body->setNumIterations(iterations);
	rope_body->setLinearDamping(linear_damping);
	rope_body->setRigidity(rigidity);
	rope_body->setLinearRestitution(linear_restitution);
	rope_body->setAngularRestitution(angular_restitution);

	// restore particles position and velocity
	num_particles = rope_body->getNumParticles();
	int size = min(num_particles, particles_position.size());
	for (int i = 0; i < size; i++)
	{
		rope_body->setParticlePosition(i, particles_position[i]);
		rope_body->setParticleVelocity(i, particles_velocity[i]);
	}

	if (num_particles > 0)
	{
		if (bag_attached && bag_body)
			rope_body->setParticlePosition(0, bag_anchor->getWorldPosition());
		rope_body->setParticlePosition(num_particles - 1, anchor_body->getPosition());
	}

	// create anchor joint
	anchor_joint = JointParticles::create(anchor_body, rope_body, anchor_body->getPosition(),
		vec3(0.01f));
	anchor_joint->setNumIterations(iterations);

	// create bag joint
	if (bag_attached)
		create_bag_joint();
}

void SlingRope::update_internal_mesh()
{
	if (!base_rope_obj)
	{
		base_rope_obj = ObjectMeshDynamic::create();
		base_rope_obj->setCastShadow(false, 0);
		base_rope_obj->setCastWorldShadow(false, 0);
		base_rope_obj->setWorldPosition(anchor_obj->getWorldPosition() + Vec3_down * rope_length);
		base_rope_obj->setMaterial(invisible_base_mat.get(), "*");
	}

	base_rope_obj->clearSurfaces();
	base_rope_obj->clearIndices();
	base_rope_obj->clearVertex();

	base_rope_obj->addVertex(dir1 * radius);
	base_rope_obj->addVertex(dir2 * radius);
	base_rope_obj->addVertex(dir3 * radius);

	base_rope_obj->addIndex(0);
	base_rope_obj->addIndex(1);
	base_rope_obj->addIndex(2);

	for (int i = 0; i < num_segments; i++)
	{
		vec3 up(0.0f, 0.0f, segment_length * (i + 1));

		base_rope_obj->addVertex(dir1 * radius + up);
		base_rope_obj->addVertex(dir2 * radius + up);
		base_rope_obj->addVertex(dir3 * radius + up);

		int index_offset = 3 * i;

		base_rope_obj->addIndex(index_offset + 0);
		base_rope_obj->addIndex(index_offset + 1);
		base_rope_obj->addIndex(index_offset + 3);

		base_rope_obj->addIndex(index_offset + 4);
		base_rope_obj->addIndex(index_offset + 3);
		base_rope_obj->addIndex(index_offset + 1);

		base_rope_obj->addIndex(index_offset + 4);
		base_rope_obj->addIndex(index_offset + 1);
		base_rope_obj->addIndex(index_offset + 2);

		base_rope_obj->addIndex(index_offset + 5);
		base_rope_obj->addIndex(index_offset + 4);
		base_rope_obj->addIndex(index_offset + 2);

		base_rope_obj->addIndex(index_offset + 5);
		base_rope_obj->addIndex(index_offset + 2);
		base_rope_obj->addIndex(index_offset + 0);

		base_rope_obj->addIndex(index_offset + 3);
		base_rope_obj->addIndex(index_offset + 5);
		base_rope_obj->addIndex(index_offset + 0);
	}

	base_rope_obj->addIndex(num_segments * 3);
	base_rope_obj->addIndex(num_segments * 3 + 1);
	base_rope_obj->addIndex(num_segments * 3 + 2);

	base_rope_obj->setMaterial(invisible_base_mat.get(), "*");

	base_rope_obj->updateTangents();
	base_rope_obj->updateBounds();
	base_rope_obj->flushVertex();
	base_rope_obj->flushIndices();
}

void SlingRope::show_debug()
{
	if (rope_obj && rope_obj->getNumVertex())
	{
		rope_obj->clearVertex();
		rope_obj->clearIndices();

		rope_obj->updateTangents();
		rope_obj->updateBounds();
		rope_obj->flushVertex();
		rope_obj->flushIndices();
	}

	if (!rope_body)
		return;

	for (int i = 1; i < num_particles; i++)
	{
		Visualizer::renderVector(particles_position[i - 1], particles_position[i], vec4_red);
		Visualizer::renderSolidSphere(radius, translate(particles_position[i - 1]), vec4_blue);
	}

	if (bag_body)
		Visualizer::renderMessage3D(bag_anchor->getWorldPosition() + Vec3_right, vec3_zero,
			String::ftoa(bag_body->getMass()), vec4_blue, 1, 20);
}

void SlingRope::refresh_mesh()
{
	if (!rope_body)
		return;

	// collect particles transforms for visible rope mesh
	Vector<Mat4> transforms;
	transforms.resize(num_particles);
	for (int i = 1; i < num_particles - 1; i++)
	{
		Vec3 prev = particles_position[i - 1];
		Vec3 curr = particles_position[i];
		Vec3 next = particles_position[i + 1];

		Vec3 forward = (next - curr).normalize();
		Vec3 up = cross(forward, prev - curr).normalize();

		if (i != 1)
		{
			if (dot(up, transforms[i - 1].getAxisZ()) <= 0.0f)
				up = -up;
		}

		Vec3 right = cross(forward, up).normalize();

		Mat4 t;
		t.setColumn3(0, right);
		t.setColumn3(1, forward);
		t.setColumn3(2, up);
		t.setColumn3(3, curr);
		transforms[i] = t;
	}

	if (num_particles > 1)
	{
		Vec3 forward = (particles_position[1] - particles_position[0]).normalize();
		Vec3 up = transforms[1].getAxisZ();

		// TODO ????
		//		if (dot(up, transforms[1].getAxisZ()) < 0)
		//			up = -up;

		Vec3 right = cross(forward, up).normalize();

		transforms[0].setColumn3(0, right);
		transforms[0].setColumn3(1, forward);
		transforms[0].setColumn3(2, up);
		transforms[0].setColumn3(3, particles_position[0]);
	}

	transforms[num_particles - 1] = transforms[0];
	transforms[num_particles - 1].setColumn3(3, particles_position[num_particles - 1]);

	rope_obj->clearVertex();
	rope_obj->clearIndices();

	// vertices
	for (int i = 0; i < num_particles; i++)
	{
		Mat4 t = transforms[i];
		vec3 p(t.getTranslate());

		vec3 p1 = p - vec3(t.getAxisX() * radius);
		vec3 p2 = p + vec3(t.getAxisX() * radius);

		rope_obj->addVertex(rope_obj->getIWorldTransform() * p1);
		rope_obj->addVertex(rope_obj->getIWorldTransform() * p2);

		rope_obj->addVertex(rope_obj->getIWorldTransform() * p1);
		rope_obj->addVertex(rope_obj->getIWorldTransform() * p2);
	}

	// indices
	for (int i = 0; i < num_particles - 1; i++)
	{
		int index_offset = 4 * i + 2;

		rope_obj->addIndex(index_offset + 0);
		rope_obj->addIndex(index_offset + 1);
		rope_obj->addIndex(index_offset + 3);

		rope_obj->addIndex(index_offset + 0);
		rope_obj->addIndex(index_offset + 3);
		rope_obj->addIndex(index_offset + 2);
	}

	rope_obj->updateTangents();
	rope_obj->updateBounds();
	rope_obj->flushVertex();
	rope_obj->flushIndices();
}

bool SlingRope::create_bag_joint()
{
	if (!rope_body || !bag_body)
		return false;

	if (bag_joint)
		bag_joint.deleteLater();

	bag_joint = JointParticles::create(bag_body, rope_body, bag_anchor->getWorldPosition(),
		vec3(0.01f));
	bag_joint->setNumIterations(iterations);
	bag_joint->setCollision(1);
	bag_joint->setLinearSoftness(joint_linear_softness);
	bag_joint->setAngularSoftness(joint_angular_softness);
	bag_joint->setLinearRestitution(joint_linear_restitution);
	bag_joint->setAngularRestitution(joint_angular_restitution);
	return true;
}

void SlingRope::collect_particles_data()
{
	if (!rope_body)
		num_particles = 0;
	else
		num_particles = rope_body->getNumParticles();

	particles_position.resize(num_particles);
	particles_velocity.resize(num_particles);

	if (num_particles < 1)
		return;

	for (int i = 0; i < num_particles; i++)
	{
		particles_position[i] = rope_body->getParticlePosition(i);
		particles_velocity[i] = rope_body->getParticleVelocity(i);
	}

	if (bag_attached && bag_body)
		particles_position[0] = bag_anchor->getWorldPosition();
}
