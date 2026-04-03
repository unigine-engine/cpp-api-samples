// Demonstrates terrain height slicing using LandscapeLayerMap. The HeightSlicer
// component copies height data from a source terrain layer and applies a threshold
// cut, modifying a target layer's albedo and height based on the slice level.

#include "CombinedSample.h"

#include <UnigineInput.h>

REGISTER_COMPONENT(CombinedSample);

using namespace Unigine;
using namespace Math;

void CombinedSample::init()
{
	init_gui();

	// static_ptr_cast converts NodePtr to specific type (LandscapeLayerMapPtr)
	// Use when the type is known at compile time (faster than dynamic cast)
	source_lmap = static_ptr_cast<LandscapeLayerMap>(source_lmap_param.get());
	target_lmap = static_ptr_cast<LandscapeLayerMap>(target_lmap_param.get());

	// Retrieve HeightSlicer component from the referenced node
	height_slicer = getComponent<HeightSlicer>(height_slicer_param);
	height_slicer->setSourceLmap(source_lmap);
	height_slicer->setTargetLmap(target_lmap);
	// Connect the texture draw callback for async rendering
	height_slicer->enable();

	// Apply initial slice height and trigger the slicing operation
	height_slicer->setSliceHeight(slice_height);
	height_slicer->run();
}

void CombinedSample::shutdown()
{
	shutdown_gui();
	// Disconnect callbacks to prevent dangling references
	height_slicer->disable();
}

void CombinedSample::init_gui()
{
	sample_description_window.createWindow();

	auto window = sample_description_window.getWindow();

	// Slider adjusts the height threshold for terrain slicing
	// Values below threshold are cut away, revealing the target layer
	sample_description_window.addFloatParameter("Slice height", "slice height", 1.f, 0.0f, 1.f,
		[this](float v) {
			height_slicer->setSliceHeight(v);
			// Trigger re-computation with new threshold
			height_slicer->run();
		});
}

void CombinedSample::shutdown_gui()
{
	sample_description_window.shutdown();
}
