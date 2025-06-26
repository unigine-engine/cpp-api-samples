#include "MicroprofilerSample.h"
#include "UnigineEngine.h"
#include "UnigineWindowManager.h"
#include <UnigineProfiler.h>

REGISTER_COMPONENT(MicroprofilerSample);

using namespace Unigine;

void MicroprofilerSample::init()
{
	StringStack<> description = "";
	if (String(Profiler::getMicroprofileUrl()) == "")
	{
		WindowManager::dialogError("Warning", "Microprofiler is not available!");
		description = "<font color=\"#de4a14\"><p>Microprofiler is not compiled.</p>"
					  "<p>Use development-release binaries.</p></font>";
	}
	else
	{
		description = String::format("<p>Microprofiler url - <font color=\"#de4a14\">%s</font></p>",
			Profiler::getMicroprofileUrl());
	}

	sample_description_window.createWindow();
	
	const auto& parameterGB = sample_description_window.getParameterGroupBox();
	auto label = WidgetLabel::create(description);
	label->setFontRich(1);
	parameterGB->addChild(label);

	previous_bg_update = Engine::get()->getBackgroundUpdate();
	Engine::get()->setBackgroundUpdate(Unigine::Engine::BACKGROUND_UPDATE_RENDER_NON_MINIMIZED);
}

void MicroprofilerSample::shutdown()
{
	Engine::get()->setBackgroundUpdate(previous_bg_update);
	sample_description_window.shutdown();
}
