#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"
#include "../../utils/ScreenshotMaker.h"
#include <UnigineComponentSystem.h>

class ScreenshotAdvancedSample
	: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ScreenshotAdvancedSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(String, base_path, "screenshots");
	PROP_PARAM(String, name_suffix_fmt, "%d_%m_%y-%H_%M_%S");
	PROP_PARAM(Switch, format, Utils::ScreenshotMaker::FORMAT_TGA, Utils::ScreenshotMaker::STR_FORMAT_FOR_SWITCH);

private:
	void init();
	void update();
	void shutdown();

	Unigine::EngineWindowPtr createWindowWithCamera(const Unigine::String &title, const Unigine::Math::ivec2 &windowPosition,
		const Unigine::Math::Vec3 &cameraPosition, const Unigine::Math::vec3 &cameraDirection);

	Unigine::String generatePath(const Unigine::String& prefix = "") const;

private:
	SampleDescriptionWindow description_window;
	Unigine::Vector<Unigine::EngineWindowPtr> windows;
};
