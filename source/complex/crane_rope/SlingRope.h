#pragma once
#include <UnigineComponentSystem.h>

class SlingRope : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SlingRope, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// rope settings
	PROP_PARAM(Float, radius, 0.05f, "Radius", "Radius", "Rope");
	PROP_PARAM(Float, segment_length, 0.3f, "Segment Length", "Max segment length", "Rope");
	PROP_PARAM(Float, mass, 10.0f, "Mass", "Mass of rope without tension", "Rope");
	PROP_PARAM(Float, rigidity, 0.5f, "Rigidity", "Rope rigidity", "Rope");
	PROP_PARAM(Float, linear_damping, 0.2f, "Linear Damping", "Linear damping of rope", "Rope");
	PROP_PARAM(Float, linear_restitution, 1.0f, "Linear Restitution", "Rope stiffness", "Rope");
	PROP_PARAM(Float, angular_restitution, 1.0f, "Angular Restitution", "Rope stiffness", "Rope");
	PROP_PARAM(Int, iterations, 8, "Iterations", "Number of iterations", "Rope");
	PROP_PARAM(Float, min_tension_angle, 15.0f, "Min Tension Angle",
		"Angle between segments to check tension", "Rope");
	PROP_PARAM(Float, min_tension_length, 1.5f, "Min Tension Length",
		"Minimum rope length for which tension parameters are guaranteed to be applied", "Rope");
	PROP_PARAM(Float, tension_mass_multiplier, 1.5f, "Tension Mass Multiplier",
		"Used to increase mass of rope relative to bag for stable behaviour in tension state",
		"Rope");
	PROP_PARAM(Material, rope_mat, "Rope Material", "Material for visible rope mesh", "Rope");
	PROP_PARAM(Material, invisible_base_mat, "Rope Base Material",
		"Invisible material for rope base", "Rope");

	// bag joint params
	PROP_PARAM(Float, joint_linear_softness, 1.0f, "Linear Softness", "Joint softness", "Joint");
	PROP_PARAM(Float, joint_angular_softness, 1.0f, "Angular Softness", "Joint softness", "Joint");
	PROP_PARAM(Float, joint_linear_restitution, 0.9f, "Linear Restitution", "Joint stiffness",
		"Joint");
	PROP_PARAM(Float, joint_angular_restitution, 0.9f, "Angular Restitution", "Joint stiffness",
		"Joint");

	// bag
	PROP_PARAM(Node, bag_node, "Bag Node", "Bag node", "Bag");
	PROP_PARAM(Node, bag_anchor_node, "Bag Anchor",
		"Anchor node where rope will be attached to bag", "Bag");

	void setLength(float length);
	float getLength() const { return rope_length; }

	void attachBag();
	void detachBag();

	void setBagMass(float mass);
	float getBagMass() const;

	void setTensionCompensationEnabled(bool enable);
	bool getTensionCompensationEnabled() { return tension_enabled; }

	void setDebugEnabled(bool enable);
	bool getDebugEnabled() { return debug_enabled; }

private:
	void init();
	void update();
	void shutdown();

	// check if rope body needs tension compensation
	bool check_tension();

	// change rope segments number and update BodyRope
	void change_num_segments(int num);
	// rebuild rope mesh with new number of segments
	void update_internal_mesh();
	void show_debug();
	// update visible rope mesh respective to base rope state
	void refresh_mesh();
	bool create_bag_joint();
	void collect_particles_data();

private:
	bool debug_enabled = false;
	bool tension_enabled = false;

	// crane anchor
	Unigine::ObjectDummyPtr anchor_obj;
	Unigine::BodyDummyPtr anchor_body;
	Unigine::JointParticlesPtr anchor_joint;

	// rope parameters
	float rope_length = 0.0f;
	int num_segments = 0;
	int num_particles = 0;
	float segments_offset = 0.0f;
	float min_tension_cos = 0.966f;

	Unigine::Vector<Unigine::Math::Vec3> particles_position;
	Unigine::Vector<Unigine::Math::vec3> particles_velocity;

	// rope base
	Unigine::ObjectMeshDynamicPtr base_rope_obj;
	Unigine::BodyRopePtr rope_body;
	// visible rope object
	Unigine::ObjectMeshDynamicPtr rope_obj;

	const Unigine::Math::vec3 dir1 = Unigine::Math::vec3_forward;
	const Unigine::Math::vec3 dir2
		= (Unigine::Math::vec3_left + Unigine::Math::vec3_back).normalize();
	const Unigine::Math::vec3 dir3
		= (Unigine::Math::vec3_right + Unigine::Math::vec3_back).normalize();

	// bag
	Unigine::NodePtr bag_anchor;
	Unigine::BodyRigidPtr bag_body;
	Unigine::JointParticlesPtr bag_joint;
	bool bag_attached = true;
};
