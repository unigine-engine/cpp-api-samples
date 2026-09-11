#pragma once

#include <UnigineWidgets.h>

#include <functional>

// UI helpers in the style of SampleDescriptionWindow parameters, adding
// widgets to an arbitrary container. The value widget is returned so it can
// be refreshed or tweaked externally.

Unigine::WidgetSliderPtr add_float_parameter(Unigine::EventConnections &connections, const Unigine::WidgetGridBoxPtr &grid,
	const char *name, const char *tooltip, float default_value, float min_value, float max_value,
	std::function<void(float)> on_change);

Unigine::WidgetSliderPtr add_int_parameter(Unigine::EventConnections &connections, const Unigine::WidgetGridBoxPtr &grid,
	const char *name, const char *tooltip, int default_value, int min_value, int max_value,
	std::function<void(int)> on_change);

Unigine::WidgetComboBoxPtr add_switch_parameter(Unigine::EventConnections &connections, const Unigine::WidgetGridBoxPtr &grid,
	const char *name, const char *tooltip, int default_value, const Unigine::Vector<const char *> &values,
	std::function<void(int)> on_change);

Unigine::WidgetCheckBoxPtr add_bool_parameter(Unigine::EventConnections &connections, const Unigine::WidgetGridBoxPtr &grid,
	const char *name, const char *tooltip, bool default_value, std::function<void(bool)> on_change);

Unigine::WidgetCheckBoxPtr add_bool_parameter(Unigine::EventConnections &connections, const Unigine::WidgetGroupBoxPtr &group,
	const char *name, const char *tooltip, bool default_value, std::function<void(bool)> on_change);

Unigine::WidgetButtonPtr add_button(Unigine::EventConnections &connections, const Unigine::WidgetPtr &parent,
	const char *name, const char *tooltip, std::function<void()> on_click);

// a labeled edit line with a small action button in the third grid column
Unigine::WidgetEditLinePtr add_string_field_with_button(Unigine::EventConnections &connections, const Unigine::WidgetGridBoxPtr &grid,
	const char *name, const char *tooltip, const char *default_value,
	const char *button_name, const char *button_tooltip, std::function<void()> on_click);
