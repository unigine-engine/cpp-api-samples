#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

class Manipulators;

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

	Manipulators* component = nullptr;

	void x_axis_rotation_check_box_callback();
	void y_axis_rotation_check_box_callback();
	void z_axis_rotation_check_box_callback();
	void x_axis_translation_check_box_callback();
	void y_axis_translation_check_box_callback();
	void z_axis_translation_check_box_callback();
	void x_axis_scale_check_box_callback();
	void y_axis_scale_check_box_callback();
	void z_axis_scale_check_box_callback();

	void local_basis_button_callback();
	void world_basis_button_callback();

	Unigine::WidgetCheckBoxPtr x_axis_rotation_check_box;
	Unigine::WidgetCheckBoxPtr y_axis_rotation_check_box;
	Unigine::WidgetCheckBoxPtr z_axis_rotation_check_box;
	Unigine::WidgetCheckBoxPtr x_axis_translation_check_box;
	Unigine::WidgetCheckBoxPtr y_axis_translation_check_box;
	Unigine::WidgetCheckBoxPtr z_axis_translation_check_box;
	Unigine::WidgetCheckBoxPtr x_axis_scale_check_box;
	Unigine::WidgetCheckBoxPtr y_axis_scale_check_box;
	Unigine::WidgetCheckBoxPtr z_axis_scale_check_box;
	Unigine::WidgetButtonPtr local_basis_button;
	Unigine::WidgetButtonPtr world_basis_button;

	bool is_local_basis;
	SampleDescriptionWindow sample_description_window;
	Unigine::Input::MOUSE_HANDLE previous_handle;
};