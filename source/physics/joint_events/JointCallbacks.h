// Demonstrates joint breaking events using a destructible bridge structure.
// Bridge sections are connected by hinge joints with force/torque limits.
// When stress exceeds limits, joints break and trigger visual feedback.

#pragma once

#include <UnigineComponentSystem.h>

class JointCallbacks : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(JointCallbacks, Unigine::ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Number of segments in the bridge chain
	PROP_PARAM(Int, bridge_sections, 14);
	// Material applied to segments after their joint breaks
	PROP_PARAM(Material, broken_materal);
	// Material applied to intact segments still connected
	PROP_PARAM(Material, joint_materal);
	// Mesh asset for bridge section blocks
	PROP_PARAM(File, mesh_file);

private:
	void init();
	void shutdown();

	// Callback invoked when joint force/torque exceeds limits
	void broken_callback(const Unigine::JointPtr &joint);

private:
	Unigine::MaterialPtr broken_mat;
	Unigine::MaterialPtr joint_mat;

	// All created objects for cleanup
	Unigine::Vector<Unigine::NodePtr> objects;
	// Manages all joint event connections
	Unigine::EventConnections joint_connections;

	// Spacing between adjacent bridge sections
	float space = 1.1f;
};
