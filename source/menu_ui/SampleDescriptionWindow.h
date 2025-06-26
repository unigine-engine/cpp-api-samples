#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>


class SampleDescriptionWindow: public Unigine::EventConnections
{
public:
	SampleDescriptionWindow(){};
	~SampleDescriptionWindow(){};

	void createWindow(int align = Unigine::Gui::ALIGN_LEFT, int width = 400);
	void shutdown();


	Unigine::WidgetLabelPtr addLabel(const char* label_text);
	Unigine::WidgetSliderPtr addFloatParameter(const char *name, const char *tooltip, float default_value,
		float min_value, float max_value, std::function<void(float)> on_change);
	Unigine::WidgetSliderPtr addIntParameter(const char *name, const char *tooltip, int default_value, int min_value,
		int max_value, std::function<void(int)> on_change);
	Unigine::WidgetCheckBoxPtr addBoolParameter(const char *name, const char *tooltip, bool default_value, std::function<void(bool)> on_change);

	void addParameterSpacer();

	void setStatus(const char *status);

	const Unigine::WidgetGroupBoxPtr &getParameterGroupBox();
	const Unigine::WidgetWindowPtr &getWindow() const { return w_main_window; };

private:
	Unigine::WidgetWindowPtr w_main_window;
	Unigine::WidgetGroupBoxPtr w_about_group;
	Unigine::WidgetLabelPtr w_about_lbl;
	Unigine::WidgetGroupBoxPtr w_controls_group;
	Unigine::WidgetLabelPtr w_controls_lbl;
	Unigine::WidgetGroupBoxPtr w_parameters_group;
	Unigine::WidgetGridBoxPtr w_parameters_grid;
	Unigine::WidgetGroupBoxPtr w_status_group;
	Unigine::WidgetLabelPtr w_status_lbl;

	void init_parameter_box();
	void init_status_box();
};
