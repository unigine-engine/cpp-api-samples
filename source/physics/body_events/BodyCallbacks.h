#pragma once

#include <UnigineComponentSystem.h>

class BodyCallbacks : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(BodyCallbacks, Unigine::ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// tower params
	PROP_PARAM(Float, space, 1.2);
	PROP_PARAM(Int, tower_level, 10);
	// different materials for different callbacks
	PROP_PARAM(Material, frozen_materal);
	PROP_PARAM(Material, position_materal);
	// mesh used to build a tower
	PROP_PARAM(File, mesh_file);

private:
	void init();
	void shutdown();

	void frozen_callback(const Unigine::BodyPtr &body);
	void position_callback(const Unigine::BodyPtr &body);
	void contact_callback(const Unigine::BodyPtr &body);

private:
	Unigine::MaterialPtr frozen_mat;
	Unigine::MaterialPtr position_mat;

	Unigine::Vector<Unigine::NodePtr> objects;
	Unigine::EventConnections body_connections;
};
