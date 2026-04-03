// Loads meshes and textures asynchronously via AsyncQueue. Meshes are polled
// each frame; textures use event callbacks. Creates scene objects and GUI sprites.

#include "AsyncQueueSample.h"

#include <UnigineAsyncQueue.h>
#include <UnigineConsole.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(AsyncQueueSample)

// Resources are queued for background loading; texture event is subscribed.
void AsyncQueueSample::init()
{
	// Queue meshes for async loading and store request IDs
	for (int i = 0; i < meshes.size(); ++i)
	{
		const char *name = meshes[i];
		AsyncLoadRequest request;
		request.name = name;
		request.id = AsyncQueue::loadMesh(name);
		mesh_load_requests.push_back(request);
	}

	// Queue textures for async loading (handled by callback)
	for (int i = 0; i < textures.size(); ++i)
	{
		AsyncQueue::loadImage(textures[i]);
	}

	// Subscribe to texture load events
	AsyncQueue::getEventImageLoaded().connect(this, &AsyncQueueSample::image_loaded_callback);

	Console::setOnscreen(true);
}

// Completed mesh requests are processed and scene objects are created.
void AsyncQueueSample::update()
{
	// Check the status of all mesh load requests
	for (int i = 0; i < mesh_load_requests.size(); ++i)
	{
		const AsyncLoadRequest &request = mesh_load_requests[i];
		if (!AsyncQueue::checkMesh(request.id))
		{
			continue;
		}

		// Take mesh from AsyncQueue once it is ready
		MeshPtr mesh = AsyncQueue::takeMesh(request.id);

		if (mesh != nullptr)
		{
			// Create ObjectMeshDynamic object from loaded mesh
			const ObjectMeshDynamicPtr object_mesh_dynamic = ObjectMeshDynamic::create(mesh);

			// Place objects sequentially along the X axis
			constexpr Scalar initial_pos = -5.;
			constexpr Scalar step = 5.;
			object_mesh_dynamic->setPosition(
				Vec3(initial_pos + static_cast<float>(objects_placed) * step, 0., 0.));
			objects_placed++;

			// Remove request from AsyncQueue and local tracking
			AsyncQueue::removeMesh(request.id);
			Log::message("Loaded mesh \"%s\"\n", request.name.get());
			mesh_load_requests.remove(i--);
		}
	}
}

// GUI sprites are removed and console is hidden.
void AsyncQueueSample::shutdown()
{
	// Remove all GUI sprites and hide console
	for (auto &sprite : sprites)
	{
		sprite.deleteLater();
	}

	Console::setOnscreen(false);
}

// Called when a texture finishes loading
void AsyncQueueSample::image_loaded_callback(const char *name, int id)
{
	auto loaded_image = AsyncQueue::takeImage(id);
	if (!loaded_image)
	{
		return;
	}

	AsyncQueue::removeImage(id);
	Log::message("Image \"%s\" loaded\n", name);

	// Create GUI sprite to display the loaded image
	auto main_window = WindowManager::getMainWindow();
	auto gui = main_window->getGui();

	auto sprite = WidgetSprite::create(gui);
	sprites.push_back(sprite);
	gui->addChild(sprite, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);
	sprite->setImage(loaded_image);
	sprite->setWidth(100);
	sprite->setHeight(100);

	ivec2 initial_sprite_position = ivec2(0, main_window->getSize().y - 200);

	// Place sprites sequentially across the window
	auto new_pos = ivec2(initial_sprite_position.x + sprites.size() * 100,
		initial_sprite_position.y);
	sprite->setPosition(new_pos.x, new_pos.y);
}
