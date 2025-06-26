#pragma once

#include <UnigineLog.h>
#include <UnigineViewport.h>
#include <UnigineWindowManager.h>

namespace Utils
{

class ScreenshotMaker
{
public:
	enum FORMAT
	{
		FORMAT_TGA = 0,
		FORMAT_PNG,
		FORMAT_JPG,
		NUM_FORMATS
	};

	static const char *STR_FORMAT_FOR_SWITCH;
	static const char *STR_FORMAT[NUM_FORMATS];

	static void grab(Unigine::CameraPtr camera, const Unigine::String &path, const Unigine::Math::ivec2 &size, int warmup);
	static void grab(const Unigine::EngineWindowPtr &window, const Unigine::String &path, bool withGui);

private:
	static void onRender(Unigine::EngineWindowPtr window, Unigine::String path);

	static void save(Unigine::ImagePtr image, Unigine::String path);
};

}
