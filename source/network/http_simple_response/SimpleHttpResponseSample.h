#pragma once

#include <UnigineComponentSystem.h>

#include "../../utils/network/HttpServer.h"

class SimpleHttpResponseSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SimpleHttpResponseSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(String, host_ip, "127.0.0.1");
	PROP_PARAM(Int, port, 8080);
	PROP_PARAM(Node, pedestal_text);

private:
	void init_gui();

private:
	void init();
	void shutdown();

	HttpServer* server = nullptr;
};
