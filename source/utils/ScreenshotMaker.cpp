#include "ScreenshotMaker.h"
#include "EventTask.h"
#include "MutableLambda.h"
#include <UnigineEngine.h>
#include <UnigineViewport.h>
#include <UnigineFileSystem.h>

using namespace Unigine;

namespace Utils
{

const char* ScreenshotMaker::STR_FORMAT_FOR_SWITCH = "tga,png,jpg";
const char* ScreenshotMaker::STR_FORMAT[NUM_FORMATS] = { "tga", "png", "jpg" };

void ScreenshotMaker::grab(Unigine::CameraPtr camera, const Unigine::String &path, const Math::ivec2 &size, int warmup)
{
	if (!camera)
	{
		Log::error("ScreenshotMaker: can not grab screenshot from null camera\n");
		return;
	}

	auto viewport = Viewport::create();
	auto texture = Texture::create();

	Log::message("ScreenshotMaker: grab started from camera...\n");
	viewport->appendSkipFlags(Viewport::SKIP_VELOCITY_BUFFER);
	viewport->renderTexture2D(camera, texture, size.x, size.y);
	viewport->removeSkipFlags(Viewport::SKIP_VELOCITY_BUFFER);

	Task<>::addTask(Engine::get()->getEventEndWorldUpdate(), Utils::mutableLambda([viewport, camera, texture, path, size, warmup, warmupCounter = 0]() mutable {
		Log::message("Warming up camera: %i\n", warmupCounter);

		float exposureAdaptation = Render::getExposureAdaptation();
		Render::setExposureAdaptation(0);
		viewport->renderTexture2D(camera, texture, size.x, size.y);
		Render::setExposureAdaptation(exposureAdaptation);

		if (++warmupCounter < warmup)
			return true;

		Render::asyncTransferTextureToImage(nullptr, MakeCallback(&ScreenshotMaker::save, path), texture);

		return false;
	}));
}

void ScreenshotMaker::grab(const Unigine::EngineWindowPtr &window, const Unigine::String &path, bool withGui)
{
	if (!window)
	{
		Log::error("ScreenshotMaker: can not grab screenshot from null window\n");
		return;
	}

	auto globalParentWindow = window;
	if (window->isNested())
	{
		globalParentWindow = window->getGlobalParentGroup();

		auto w = window;
		auto parentGroup = w->getParentGroup();
		while (parentGroup.isValid())
		{
			int tab = parentGroup->getNestedWindowIndex(w);
			parentGroup->setCurrentTab(tab);

			w = parentGroup;
			parentGroup = parentGroup->getParentGroup();
		}
	}

	if (withGui)
	{
		Task<>::addSingleshotTask(globalParentWindow->getEventFuncEndRender(), &ScreenshotMaker::onRender, globalParentWindow, path);
	}
	else
	{
		if (window->getType() == Unigine::EngineWindow::ENGINE_WINDOW_GROUP)
		{
			Log::warning("ScreenshotMaker: groups don't support screenshots without gui\n");
			return;
		}

		if (window->isSeparate())
			Task<>::addSingleshotTask(globalParentWindow->getEventFuncRender(), &ScreenshotMaker::onRender, globalParentWindow, path);
		else
			Task<>::addSingleshotTask(window->getEventFuncRender(), &ScreenshotMaker::onRender, window, path);
	}
}

void ScreenshotMaker::onRender(Unigine::EngineWindowPtr window, Unigine::String path)
{
	Log::message("ScreenshotMaker: grab from window was started\n");

	auto windowSize = window->getClientRenderSize();

	auto texture = Render::getTemporaryTexture2D(windowSize.x, windowSize.y,
		Texture::FORMAT_RGBA8, -1, nullptr,
		Render::TEXTURE_ACCESSORY_NONE,
		Render::TEXTURE_LIFETIME_FRAME_VIEWPORT, true);

	texture->copy2D();
	Render::asyncTransferTextureToImage(nullptr, MakeCallback(&ScreenshotMaker::save, path), texture);
}

void ScreenshotMaker::save(Unigine::ImagePtr image, Unigine::String path)
{
	auto absolutePath = FileSystem::getAbsolutePath(path.get());
	image->convertToFormat(Image::FORMAT_RGB8);
	image->save(absolutePath.get());
	Log::message("ScreenshotMaker: grab from window was ended. Path: %s\n", absolutePath.get());
}

}
