#include "HttpImageRequestSample.h"

#include <UnigineWidgets.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(HttpImageRequestSample);
REGISTER_COMPONENT(HttpImageToTexture);

using namespace Unigine;
using namespace Math;

void HttpImageRequestSample::init()
{
	ThreadQueue::get().run();
}

void HttpImageRequestSample::shutdown()
{
	ThreadQueue::get().stop();
}

void HttpImageToTexture::init()
{
	String host_str = host.get();
	String args_str = args.get();
	request = std::make_unique<AsyncTask<httplib::Result>>([host_str, args_str]() -> httplib::Result {
		httplib::Client cli(host_str.get());
		return cli.Get(args_str.get());
	});
}

void HttpImageToTexture::update()
{
	if (request && request->isCompleted())
	{
		if (httplib::Result &result = request->get())
		{
			if (result->status == 200)
			{
				unsigned char *data = (unsigned char *)result->body.data();

				image = Image::create();
				bool loaded = image->load(data, toInt(result->body.size()));
				if (!loaded)
				{
					Log::error("HttpImageToTexture can not load image\n");
					FilePtr file = File::create("wrong_image", "wb");
					file->write(data, result->body.size());
					file->close();
					Log::error("HttpImageToTexture save loaded image to 'data/wrong_image' file\n");
				}
				ObjectPtr object = checked_ptr_cast<Object>(node);
				if (!object)
					Log::error("HttpImageToTexture can be assigned only to Objects: %s(%d) is %s\n", node->getName(), node->getID(), node->getTypeName());
				auto mat = object->getMaterialInherit(surface.get());
				if (!mat)
					Log::error("HttpImageToTexture can not find any material on %d surface of %s(%d)\n", surface.get(), node->getName(), node->getID());
				int slot = mat->findTexture(texture.get());
				if (!mat)
					Log::error("HttpImageToTexture can not find texture slot '%s' in material on %d surface of %s(%d)\n", texture.get(), surface.get(), node->getName(), node->getID());
				mat->setTextureImage(slot, image);
			} else
				Log::message("%d %s\n", result->status, result->reason.c_str());

		} else
		{
			String error = httplib::to_string(result.error()).c_str();
			Log::error("%s\n", error.get());
		}

		request.reset();
	}
}
