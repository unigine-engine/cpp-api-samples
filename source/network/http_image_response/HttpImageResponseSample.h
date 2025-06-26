#pragma once
#include <UnigineComponentSystem.h>
#include <UnigineViewport.h>
#include "../../utils/network/httplib.h"
#include <atomic>

#include "../../utils/network/HttpServer.h"

class HttpImageResponseSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(HttpImageResponseSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(String, host_ip, "127.0.0.1");
	PROP_PARAM(Int, port, 8080);
	PROP_PARAM(Node, pedestal_text);

private:
	void init_gui();

private:
	void init();
	void update();
	void shutdown();

	void grab_engine_texture();

	HttpServer *server = nullptr;
	Unigine::TexturePtr texture;
	Unigine::String png_data;
	Unigine::Mutex png_data_mutex;
	std::atomic<bool> image_ready{ false };
	std::atomic<bool> need_convert{ false };
};
