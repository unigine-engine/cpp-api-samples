// Downloads images via HTTP and applies them as textures on scene objects.
// Uses AsyncTask for non-blocking requests on background threads. The image
// is loaded from response body and assigned to a material's texture slot.

#include "HttpImageRequestSample.h"

#include <UnigineWidgets.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(HttpImageRequestSample);
REGISTER_COMPONENT(HttpImageToTexture);

using namespace Unigine;
using namespace Math;

// Background thread queue is started for async HTTP operations.
void HttpImageRequestSample::init()
{
	ThreadQueue::get().run();
}

// Background thread queue is stopped to allow clean shutdown.
void HttpImageRequestSample::shutdown()
{
	ThreadQueue::get().stop();
}

// HTTP GET request is created and queued for background execution.
void HttpImageToTexture::init()
{
	// Parameters are captured by value for thread-safe lambda execution
	String host_str = host.get();
	String args_str = args.get();
	request = std::make_unique<AsyncTask<httplib::Result>>([host_str, args_str]() -> httplib::Result {
		httplib::Client cli(host_str.get());
		return cli.Get(args_str.get());
	});
}

// Request completion is polled; downloaded image is applied to material texture.
void HttpImageToTexture::update()
{
	// Async request is checked for completion each frame
	if (request && request->isCompleted())
	{
		if (httplib::Result &result = request->get())
		{
			// HTTP 200 indicates successful download
			if (result->status == 200)
			{
				unsigned char *data = (unsigned char *)result->body.data();

				// Image is created from raw HTTP response body
				image = Image::create();
				bool loaded = image->load(data, toInt(result->body.size()));
				if (!loaded)
				{
					// Failed image data is saved to file for debugging
					Log::error("HttpImageToTexture can not load image\n");
					FilePtr file = File::create("wrong_image", "wb");
					file->write(data, result->body.size());
					file->close();
					Log::error("HttpImageToTexture save loaded image to 'data/wrong_image' file\n");
				}

				// Node is cast to Object to access material surfaces
				ObjectPtr object = checked_ptr_cast<Object>(node);
				if (!object)
				{
					Log::error("HttpImageToTexture can be assigned only to Objects: %s(%d) is %s\n", node->getName(), node->getID(), node->getTypeName());
					return;
				}

				// Inherited material is retrieved to allow per-instance modification
				auto mat = object->getMaterialInherit(surface.get());
				if (!mat)
				{
					Log::error("HttpImageToTexture can not find any material on %d surface of %s(%d)\n", surface.get(), node->getName(), node->getID());
					return;
				}

				// Texture slot index is looked up by name
				int slot = mat->findTexture(texture.get());
				if (slot == -1)
				{
					Log::error("HttpImageToTexture can not find texture slot '%s' in material on %d surface of %s(%d)\n", texture.get(), surface.get(), node->getName(), node->getID());
					return;
				}

				// Downloaded image is assigned to the material's texture slot
				mat->setTextureImage(slot, image);
			} else
				// Non-200 HTTP status is logged as informational message
				Log::message("%d %s\n", result->status, result->reason.c_str());

		} else
		{
			// HTTP client error is logged (connection failed, timeout, etc.)
			String error = httplib::to_string(result.error()).c_str();
			Log::error("%s\n", error.get());
		}

		// Request handle is released after processing
		request.reset();
	}
}
