#include "ObjectFrameSample.h"
#include "ObjectFrame.h"

#include <UnigineConsole.h>
#include <ctime>

#include "../../utils/Utils.h"

REGISTER_COMPONENT(ObjectFrameSample);

using namespace Unigine;

void ObjectFrameSample::init()
{
	sample_description_window.createWindow();

	auto param_box = sample_description_window.getParameterGroupBox();
	param_box->setText("Controls");
	WidgetButtonPtr button = WidgetButton::create("Snap Screenshot");
	button->getEventClicked().connect(*this, [this]() {
		ObjectFrame::setObjectFramesEnabled(false);
		grab_flag = true;
	});
	param_box->addChild(button, Gui::ALIGN_EXPAND);

	label = WidgetLabel::create("");
	param_box->addChild(label, Gui::ALIGN_LEFT);

	window = WindowManager::getMainWindow();
	if (window)
		window->getEventFuncEndRender().connect(*this, this,
			&ObjectFrameSample::snap_screenshot);

	ComponentSystem::get()->getComponentsInWorld(frames, true);
}

void ObjectFrameSample::shutdown()
{
	sample_description_window.shutdown();
}

void ObjectFrameSample::snap_screenshot()
{
	if (!grab_flag)
		return;
	grab_flag = false;

	TexturePtr temporary_texture = Render::getTemporaryTexture2D(window->getClientRenderSize().x,
		window->getClientRenderSize().y, Texture::FORMAT_RGBA8);
	temporary_texture->copy2D();
	ObjectFrame::setObjectFramesEnabled(true);

	time_t now = time(0);
	std::strftime(time_str.getRaw(), time_str.getMemoryUsage(), "%Y-%m-%d %H_%M_%S",
		std::localtime(&now));

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

	Render::asyncTransferTextureToImage(nullptr, MakeCallback([this](ImagePtr image) {
		if (!Render::isFlipped())
			image->flipY();

		Log::message("Saving \"%s.png\"\n", time_str.get());
		image->save(joinPaths(getWorldRootPath(), "screenshots", time_str + ".png"));

		String msg = String::format("Saved screenshot \"%s.png\"", time_str.get());
		label->setText(msg);
	}),
		temporary_texture);
	Render::releaseTemporaryTexture(temporary_texture);
}
