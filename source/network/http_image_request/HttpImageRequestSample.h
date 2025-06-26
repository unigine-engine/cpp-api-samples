#pragma once

#include <UnigineComponentSystem.h>
#include "../../utils/AsyncTasks.h"
#include "../../utils/network/httplib.h"

class HttpImageRequestSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(HttpImageRequestSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();
};

class HttpImageToTexture: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(HttpImageToTexture, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	PROP_PARAM(String, host, "eu.httpbin.org");
	PROP_PARAM(String, args, "/image/");
	PROP_PARAM(Int, surface, 0);
	PROP_PARAM(String, texture, "albedo");

private:
	void init();
	void update();

	Unigine::ImagePtr image;
	std::unique_ptr<AsyncTask<httplib::Result>> request;
};
