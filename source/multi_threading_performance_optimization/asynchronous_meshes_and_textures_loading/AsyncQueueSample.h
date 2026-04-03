#pragma once

#include <UnigineComponentSystem.h>

// Demonstrates background resource loading using AsyncQueue.
// Meshes and textures are loaded in separate threads while the main thread
// remains responsive. Completed resources are placed in the scene or displayed as sprites.
class AsyncQueueSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(AsyncQueueSample, ComponentBase)
	COMPONENT_DESCRIPTION("This component demonstrates asynchronous resource loading using AsyncQueue. "
			"It queues meshes and textures for background loading, creates scene objects when meshes are ready, "
			"and displays loaded textures as GUI sprites.")

	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

	// Lists of resources to load (set in the Editor)
	PROP_ARRAY(File, meshes)
	PROP_ARRAY(File, textures)

private:
	void init();     // Queues resources for async loading
	void update();   // Checks for completed meshes and creates objects
	void shutdown(); // Removes sprites and hides console

	// Callback for texture load completion
	void image_loaded_callback(const char *name, int id);

	// Struct for tracking mesh load requests
	struct AsyncLoadRequest
	{
		Unigine::String name;
		int id{-1};
	};
	Unigine::Vector<AsyncLoadRequest> mesh_load_requests;

	// Counter used to place meshes in the scene
	int objects_placed{0};

	// GUI sprites created from loaded textures
	Unigine::Vector<Unigine::WidgetSpritePtr> sprites;
};
