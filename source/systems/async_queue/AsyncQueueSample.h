#pragma once

#include <UnigineComponentSystem.h>

class AsyncQueueSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(AsyncQueueSample, ComponentBase)
	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

	PROP_ARRAY(File, meshes)
	PROP_ARRAY(File, textures)

private:
	void init();
	void update();
	void shutdown();

	void image_loaded_callback(const char *name, int id);

	struct AsyncLoadRequest
	{
		Unigine::String name;
		int id{-1};
	};
	Unigine::Vector<AsyncLoadRequest> mesh_load_requests;

	int objects_placed{0};

	Unigine::Vector<Unigine::WidgetSpritePtr> sprites;
};
