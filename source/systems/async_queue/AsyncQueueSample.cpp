#include "AsyncQueueSample.h"

#include <UnigineAsyncQueue.h>
#include <UnigineConsole.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(AsyncQueueSample)

void AsyncQueueSample::init()
{
	// Enqueue meshes to load
	for (int i = 0; i < meshes.size(); ++i)
	{
		const char *name = meshes[i];
		AsyncLoadRequest request;
		request.name = name;
		request.id = AsyncQueue::loadMesh(name);
		mesh_load_requests.push_back(request);
	}

	// Enqueue textures to load
	for (int i = 0; i < textures.size(); ++i)
	{
		AsyncQueue::loadImage(textures[i]);
	}

	AsyncQueue::getEventImageLoaded().connect(this, &AsyncQueueSample::image_loaded_callback);


	Console::setOnscreen(true);
}

void AsyncQueueSample::update()
{
	// check mesh load requests
	for (int i = 0; i < mesh_load_requests.size(); ++i)
	{
		const AsyncLoadRequest &request = mesh_load_requests[i];
		if (!AsyncQueue::checkMesh(request.id))
		{
			continue;
		}

		// grab loaded mesh using AsyncQueue::takeMesh() method
		MeshPtr mesh = AsyncQueue::takeMesh(request.id);

		if (mesh != nullptr)
		{
			// object with loaded mesh
			const ObjectMeshDynamicPtr object_mesh_dynamic = ObjectMeshDynamic::create(mesh);

			// set some position to this object
			constexpr Scalar initial_pos = -5.;
			constexpr Scalar step = 5.;
			object_mesh_dynamic->setPosition(
				Vec3(initial_pos + static_cast<float>(objects_placed) * step, 0., 0.));
			objects_placed++;

			// remove mesh from AsyncQueue
			AsyncQueue::removeMesh(request.id);
			Log::message("Loaded mesh \"%s\"\n", request.name.get());

			// remove request from requests collection
			mesh_load_requests.remove(i--);
		}
	}
}
void AsyncQueueSample::shutdown()
{
	for (auto &sprite : sprites)
	{
		sprite.deleteLater();
	}

	Console::setOnscreen(false);
}

void AsyncQueueSample::image_loaded_callback(const char *name, int id)
{
	auto loaded_image = AsyncQueue::takeImage(id);
	if (!loaded_image)
	{
		return;
	}

	AsyncQueue::removeImage(id);
	Log::message("Image \"%s\" loaded\n", name);

	auto main_window = WindowManager::getMainWindow();
	auto gui = main_window->getGui();

	auto sprite = WidgetSprite::create(gui);
	sprites.push_back(sprite);
	gui->addChild(sprite, Gui::ALIGN_OVERLAP);
	sprite->setImage(loaded_image);
	sprite->setWidth(100);
	sprite->setHeight(100);

	ivec2 initial_sprite_position = ivec2(0, main_window->getSize().y - 200);

	auto new_pos = ivec2(initial_sprite_position.x + sprites.size() * 100,
		initial_sprite_position.y);
	sprite->setPosition(new_pos.x, new_pos.y);
}
