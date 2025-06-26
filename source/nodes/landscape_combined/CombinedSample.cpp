#include "CombinedSample.h"

#include <UnigineInput.h>

REGISTER_COMPONENT(CombinedSample);

using namespace Unigine;
using namespace Math;

void CombinedSample::init()
{
	init_gui();

	source_lmap = static_ptr_cast<LandscapeLayerMap>(source_lmap_param.get());
	target_lmap = static_ptr_cast<LandscapeLayerMap>(target_lmap_param.get());

	height_slicer = getComponent<HeightSlicer>(height_slicer_param);
	height_slicer->setSourceLmap(source_lmap);
	height_slicer->setTargetLmap(target_lmap);
	height_slicer->enable();

	height_slicer->setSliceHeight(slice_height);
	height_slicer->run();
}

void CombinedSample::shutdown()
{
	shutdown_gui();
	height_slicer->disable();
}

void CombinedSample::init_gui()
{
	sample_description_window.createWindow();

	auto window = sample_description_window.getWindow();

	sample_description_window.addFloatParameter("Slice height", "slice height", 1.f, 0.0f, 1.f,
		[this](float v) {
			height_slicer->setSliceHeight(v);
			height_slicer->run();
		});
}

void CombinedSample::shutdown_gui()
{
	sample_description_window.shutdown();
}