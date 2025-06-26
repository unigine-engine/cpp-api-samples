#pragma once

#include <UnigineComponentSystem.h>

class JointCallbacks : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(JointCallbacks, Unigine::ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Int, bridge_sections, 14);
	// different materials for still joint and already broken sections
	PROP_PARAM(Material, broken_materal);
	PROP_PARAM(Material, joint_materal);
	// mesh used as a section
	PROP_PARAM(File, mesh_file);

private:
	void init();
	void shutdown();

	// callback invoked when joint breaks
	void broken_callback(const Unigine::JointPtr &joint);

private:
	Unigine::MaterialPtr broken_mat;
	Unigine::MaterialPtr joint_mat;

	Unigine::Vector<Unigine::NodePtr> objects;
	Unigine::EventConnections joint_connections;

	float space = 1.1f;
};
