// Captures a screenshot asynchronously at the end of frame rendering.
// Copies the color buffer to a temporary texture, transfers to Image on
// GPU thread, and saves to file. Handles Y-flip for non-flipped contexts.

#include "Screenshot.h"

using namespace Unigine;
using namespace Math;

// Registers render callback on specified window; disconnects from previous window.
void Screenshot::setWindow(const Unigine::EngineWindowPtr &window)
{
	// Remove any existing connections before switching windows
	disconnectAll();
	if (!window)
		return;

	window_ = window;
	// Subscribe to end-of-render event when frame is complete but not yet presented
	window_->getEventFuncEndRender().connect(this, &Screenshot::render);
}

// Called at end of each frame; captures screenshot when grab flag is set.
void Screenshot::render()
{
	// Skip capture unless grab() was called
	if (!grab_flag)
	{
		return;
	}
	grab_flag = false;

	// Get a temporary texture matching window dimensions for the capture
	TexturePtr temporary_texture = Render::getTemporaryTexture2D(window_->getClientSize().x,
		window_->getClientSize().y, Texture::FORMAT_RGBA8, 0);

	// Copy current color buffer contents into our temporary texture
	temporary_texture->copy2D();

	// Transfer texture data to CPU image asynchronously (non-blocking)
	Render::asyncTransferTextureToImage(nullptr, MakeCallback([](ImagePtr image) {
		// Some graphics APIs render upside-down; flip if needed for correct orientation
		if (!Render::isFlipped())
		{
			image->flipY();
		}
		image->save("screenshot.dds");
		Log::message("Screenshot saved to \"data/screenshot.dds\"\n");
	}),
		temporary_texture);

	// Return texture to pool for reuse
	Render::releaseTemporaryTexture(temporary_texture);
}
