// Demonstrates physics body event callbacks: frozen, position, and contact.
// A tower of rigid bodies is created; each body changes material based on state.
// Frozen bodies turn gray, moving bodies are highlighted, contacts are visualized.

#pragma once

#include <UnigineComponentSystem.h>

class BodyCallbacks : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(BodyCallbacks, Unigine::ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Tower construction parameters
	PROP_PARAM(Float, space, 1.2);
	PROP_PARAM(Int, tower_level, 10);
	// Material applied when body enters frozen (sleeping) state
	PROP_PARAM(Material, frozen_materal);
	// Material applied when body is moving (awake)
	PROP_PARAM(Material, position_materal);
	// Mesh asset for tower blocks
	PROP_PARAM(File, mesh_file);

private:
	void init();
	void shutdown();

	// Called when body velocity drops below frozen threshold
	void frozen_callback(const Unigine::BodyPtr &body);
	// Called each frame while body is moving
	void position_callback(const Unigine::BodyPtr &body);
	// Called when new contact point is detected
	void contact_callback(const Unigine::BodyPtr &body);

private:
	Unigine::MaterialPtr frozen_mat;
	Unigine::MaterialPtr position_mat;

	// Stores all created tower objects for cleanup
	Unigine::Vector<Unigine::NodePtr> objects;
	// Manages all event connections for batch disconnection
	Unigine::EventConnections body_connections;
};
