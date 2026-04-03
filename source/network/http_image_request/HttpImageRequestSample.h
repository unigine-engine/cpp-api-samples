// HTTP image downloading sample with asynchronous texture loading.
// HttpImageRequestSample manages the background thread queue lifecycle.
// HttpImageToTexture downloads an image from a specified URL and applies
// it to a material's texture slot on completion.

#pragma once

#include <UnigineComponentSystem.h>
#include "../../utils/AsyncTasks.h"
#include "../../utils/network/httplib.h"

// Manages background thread queue for asynchronous HTTP operations.
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

// Downloads an image via HTTP and applies it to a material texture slot.
// Request is executed asynchronously; texture is updated when download completes.
class HttpImageToTexture: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(HttpImageToTexture, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	PROP_PARAM(String, host, "eu.httpbin.org");	// HTTP server hostname
	PROP_PARAM(String, args, "/image/");		// Request path (endpoint)
	PROP_PARAM(Int, surface, 0);				// Material surface index on the object
	PROP_PARAM(String, texture, "albedo");		// Texture slot name in the material

private:
	void init();
	void update();

	Unigine::ImagePtr image;								// Downloaded image data
	std::unique_ptr<AsyncTask<httplib::Result>> request;	// Async HTTP request handle
};
