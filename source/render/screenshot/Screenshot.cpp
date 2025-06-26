#include "Screenshot.h"

using namespace Unigine;
using namespace Math;

void Screenshot::setWindow(const Unigine::EngineWindowPtr &window)
{
	disconnectAll();
	if (!window)
		return;

	window_ = window;
	window_->getEventFuncEndRender().connect(this, &Screenshot::render);
}

void Screenshot::render()
{
	if (!grab_flag)
	{
		return;
	}
	grab_flag = false;

	TexturePtr temporary_texture = Render::getTemporaryTexture2D(window_->getClientSize().x,
		window_->getClientSize().y, Texture::FORMAT_RGBA8, 0);

	// copy2D is the place where screenshot is made
	// this method copies color texture to our texture
	temporary_texture->copy2D();

	Render::asyncTransferTextureToImage(nullptr, MakeCallback([](ImagePtr image) {
		if (!Render::isFlipped())
		{
			image->flipY();
		}
		image->save("screenshot.dds");
		Log::message("Screenshot saved to \"data/screenshot.dds\"\n");
	}),
		temporary_texture);

	Render::releaseTemporaryTexture(temporary_texture);
}
