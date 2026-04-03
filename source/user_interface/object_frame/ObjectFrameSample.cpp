// Sample UI for object frame screenshot capture. On button click, frames are temporarily
// hidden, the framebuffer is captured via asyncTransferTextureToImage, and both the
// screenshot (PNG) and frame metadata (JSON) are saved with timestamped filenames.

#include "ObjectFrameSample.h"
#include "ObjectFrame.h"

#include <UnigineConsole.h>
#include <ctime>

#include "../../utils/Utils.h"

REGISTER_COMPONENT(ObjectFrameSample);

using namespace Unigine;

// UI is created and end-of-render callback is registered.
void ObjectFrameSample::init()
{
	sample_description_window.createWindow();

	// Create screenshot button that triggers capture on next frame
	auto param_box = sample_description_window.getParameterGroupBox();
	param_box->setText("Controls");
	WidgetButtonPtr button = WidgetButton::create("Snap Screenshot");
	button->getEventClicked().connect(*this, [this]() {
		// Hide frames before capture for clean screenshot
		ObjectFrame::setObjectFramesEnabled(false);
		grab_flag = true;
	});
	param_box->addChild(button, Gui::ALIGN_EXPAND);

	// Label shows filename of last saved screenshot
	label = WidgetLabel::create("");
	param_box->addChild(label, Gui::ALIGN_LEFT);

	// Register callback to capture framebuffer after render
	window = WindowManager::getMainWindow();
	if (window)
		window->getEventFuncEndRender().connect(*this, this,
			&ObjectFrameSample::snap_screenshot);

	// Collect all ObjectFrame components for metadata export
	ComponentSystem::get()->getComponentsInWorld(frames, true);
}

// UI resources are cleaned up.
void ObjectFrameSample::shutdown()
{
	sample_description_window.shutdown();
}

// Framebuffer is captured, JSON metadata is saved, and PNG is exported async.
void ObjectFrameSample::snap_screenshot()
{
	if (!grab_flag)
		return;
	grab_flag = false;

	// Copy current framebuffer to temporary texture
	TexturePtr temporary_texture = Render::getTemporaryTexture2D(window->getClientRenderSize().x,
		window->getClientRenderSize().y, Texture::FORMAT_RGBA8);
	temporary_texture->copy2D();
	// Re-enable frames now that framebuffer is captured
	ObjectFrame::setObjectFramesEnabled(true);

	// Generate timestamped filename
	time_t now = time(0);
	std::strftime(time_str.getRaw(), time_str.getMemoryUsage(), "%Y-%m-%d %H_%M_%S",
		std::localtime(&now));

	// Build JSON with metadata for all visible frames
	JsonPtr json = Json::create();
	auto array = json->addChild("entities");
	array->setArray();
	for (const auto &it : frames)
	{
		if (!it->isVisible() || !it->isEnabled())
			continue;
		auto info = it->getJsonMeta();
		array->addChild(info);
	}
	json->save(joinPaths(getWorldRootPath(), "frame_data", time_str + ".json"));

	// Async transfer texture to image and save PNG
	Render::asyncTransferTextureToImage(nullptr, MakeCallback([time_str = this->time_str, label = this->label](ImagePtr image) {
		// Flip image if render is not already flipped
		if (!Render::isFlipped())
			image->flipY();

		Log::message("Saving \"%s.png\"\n", time_str.get());
		image->save(joinPaths(getWorldRootPath(), "screenshots", time_str + ".png"));

		// Update status label
		String msg = String::format("Saved screenshot \"%s.png\"", time_str.get());
		if(label)
			label->setText(msg);
	}),
		temporary_texture);
	Render::releaseTemporaryTexture(temporary_texture);
}
