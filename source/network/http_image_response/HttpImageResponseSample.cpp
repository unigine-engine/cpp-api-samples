#include "HttpImageResponseSample.h"

REGISTER_COMPONENT(HttpImageResponseSample);

using namespace Unigine;
using namespace Math;

void HttpImageResponseSample::init_gui()
{
	auto object_text = checked_ptr_cast<ObjectText>(pedestal_text.get());
	object_text->setFontRich(1);
	object_text->setText("<p>Open:</p><p>http://" + String(host_ip.get()) + ":" + String::itoa(port) + "/unigine.png</p>");
	auto prev_pos = object_text->getPosition();
	object_text->setPosition(Vec3(object_text->getTextWidth() / 2, prev_pos.y, prev_pos.z));
}

void HttpImageResponseSample::init()
{
	Engine::get()->setBackgroundUpdate(Engine::BACKGROUND_UPDATE_RENDER_ALWAYS);

	init_gui();

	auto window = WindowManager::getMainWindow();
	window->getEventFuncEndRender().connect(this, &HttpImageResponseSample::grab_engine_texture);

	texture = Texture::create();
	texture->create2D(window->getClientRenderSize().x, window->getClientRenderSize().y, Texture::FORMAT_RGBA8, Texture::FORMAT_USAGE_RENDER);

	window->getEventResized().connectUnsafe([this]() {
		auto window = WindowManager::getMainWindow();
		texture->create2D(window->getClientRenderSize().x, window->getClientRenderSize().y, Texture::FORMAT_RGBA8, Texture::FORMAT_USAGE_RENDER);
	});

	server = new HttpServer(host_ip.get(), port.get());

	server->get("/unigine.png", [this](const httplib::Request &req, httplib::Response &res) {
		// This function will be called from another thread because it is marked as asynchronous
		need_convert = true;

		while (!image_ready && !server->isStopped())
		{
			Thread::switchThread();
		}

		if (image_ready)
		{
			ScopedLock lock{ png_data_mutex };
			res.set_content(png_data.get(), png_data.size(), "image/png");
		}
		else
		{
			res.status = 500;
			res.set_content("Failed to convert to PNG", "text/plain");
		}

		image_ready = false;

	},
	true);

	server->run();
}

void HttpImageResponseSample::update()
{
	if (need_convert)
	{
		NodePtr check_node = node;
		Render::asyncTransferTextureToImage(nullptr, MakeCallback([this, check_node](ImagePtr& transfered) {
			// This callback will be called asynchronous from another thread
			if (!check_node)
				return;

			ScopedLock lock{ png_data_mutex };

			UGUID guid = FileSystem::addVirtualFile("unigine.png");
			FileSystem::addBlobFile(guid);

			transfered->convertToFormat(Image::FORMAT_RGB8);
			transfered->save(FileSystem::guidToPath(guid).get());

			FilePtr file = File::create(FileSystem::guidToPath(guid).get(), "rb");

			png_data.resize(toInt(file->getSize()));
			file->read(png_data.getRaw(), png_data.size());

			file->close();
			FileSystem::removeBlobFile(guid);
			FileSystem::removeVirtualFile(guid);


			image_ready = true;
		}),
		texture);

		need_convert = false;
	}
}

void HttpImageResponseSample::shutdown()
{
	server->stop();
	delete server;
	server = nullptr;
}

void HttpImageResponseSample::grab_engine_texture()
{
	texture->copy2D();
}
