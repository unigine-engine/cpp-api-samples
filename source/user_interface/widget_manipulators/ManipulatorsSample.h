// UI controls for the Manipulators component. Checkboxes enable/disable individual
// axes for translation, rotation, and scale operations. Toggle buttons switch
// between world and local coordinate basis for the manipulator widgets.

#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

class Manipulators;

// Control panel for configuring WidgetManipulator axis masks and basis mode.
class ManipulatorsSample: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ManipulatorsSample, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component displays information about the sample, and demonstrates how to use the methods of the WidgetManipulators class.");

	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();

	// Reference to Manipulators component being controlled
	Manipulators* component = nullptr;

	// Callbacks for rotation axis checkboxes
	void x_axis_rotation_check_box_callback();
	void y_axis_rotation_check_box_callback();
	void z_axis_rotation_check_box_callback();
	// Callbacks for translation axis checkboxes
	void x_axis_translation_check_box_callback();
	void y_axis_translation_check_box_callback();
	void z_axis_translation_check_box_callback();
	// Callbacks for scale axis checkboxes
	void x_axis_scale_check_box_callback();
	void y_axis_scale_check_box_callback();
	void z_axis_scale_check_box_callback();

	// Callbacks for basis mode toggle buttons
	void local_basis_button_callback();
	void world_basis_button_callback();

	// Rotation axis enable/disable checkboxes
	Unigine::WidgetCheckBoxPtr x_axis_rotation_check_box;
	Unigine::WidgetCheckBoxPtr y_axis_rotation_check_box;
	Unigine::WidgetCheckBoxPtr z_axis_rotation_check_box;
	// Translation axis enable/disable checkboxes
	Unigine::WidgetCheckBoxPtr x_axis_translation_check_box;
	Unigine::WidgetCheckBoxPtr y_axis_translation_check_box;
	Unigine::WidgetCheckBoxPtr z_axis_translation_check_box;
	// Scale axis enable/disable checkboxes
	Unigine::WidgetCheckBoxPtr x_axis_scale_check_box;
	Unigine::WidgetCheckBoxPtr y_axis_scale_check_box;
	Unigine::WidgetCheckBoxPtr z_axis_scale_check_box;
	// Toggle buttons for switching between local and world basis
	Unigine::WidgetButtonPtr local_basis_button;
	Unigine::WidgetButtonPtr world_basis_button;

	// Current basis mode state (true = local, false = world)
	bool is_local_basis;
	// Helper class for sample description UI
	SampleDescriptionWindow sample_description_window;
	// Stored mouse handle to restore on shutdown
	Unigine::Input::MOUSE_HANDLE previous_handle;
};