#include "SampleWidgets.h"

using namespace Unigine;

WidgetSliderPtr add_float_parameter(EventConnections &connections, const WidgetGridBoxPtr &grid,
	const char *name, const char *tooltip, float default_value, float min_value, float max_value,
	std::function<void(float)> on_change)
{
	auto label = WidgetLabel::create(name);
	label->setWidth(100);
	label->setToolTip(tooltip);
	grid->addChild(label, Gui::ALIGN_LEFT);

	auto slider = WidgetSlider::create();
	slider->setMinValue((int)(min_value * 100));
	slider->setMaxValue((int)(max_value * 100));
	slider->setValue((int)(default_value * 100));
	slider->setWidth(200);
	slider->setButtonWidth(20);
	slider->setButtonHeight(20);
	slider->setToolTip(tooltip);
	grid->addChild(slider, Gui::ALIGN_EXPAND);

	auto value_label = WidgetLabel::create(String::ftoa(default_value, 2));
	value_label->setWidth(20);
	value_label->setToolTip(tooltip);
	grid->addChild(value_label);

	slider->getEventChanged().connect(connections, [slider, value_label, callback = std::move(on_change)]() {
		float value = slider->getValue() / 100.f;
		value_label->setText(String::ftoa(value, 2));
		callback(value);
	});

	return slider;
}

WidgetSliderPtr add_int_parameter(EventConnections &connections, const WidgetGridBoxPtr &grid,
	const char *name, const char *tooltip, int default_value, int min_value, int max_value,
	std::function<void(int)> on_change)
{
	auto label = WidgetLabel::create(name);
	label->setWidth(100);
	label->setToolTip(tooltip);
	grid->addChild(label, Gui::ALIGN_LEFT);

	auto slider = WidgetSlider::create();
	slider->setMinValue(min_value);
	slider->setMaxValue(max_value);
	slider->setValue(default_value);
	slider->setWidth(200);
	slider->setButtonWidth(20);
	slider->setButtonHeight(20);
	slider->setToolTip(tooltip);
	grid->addChild(slider, Gui::ALIGN_EXPAND);

	auto value_label = WidgetLabel::create(String::itoa(default_value));
	value_label->setWidth(20);
	value_label->setToolTip(tooltip);
	grid->addChild(value_label);

	slider->getEventChanged().connect(connections, [slider, value_label, callback = std::move(on_change)]() {
		int value = slider->getValue();
		value_label->setText(String::itoa(value));
		callback(value);
	});

	return slider;
}

WidgetComboBoxPtr add_switch_parameter(EventConnections &connections, const WidgetGridBoxPtr &grid,
	const char *name, const char *tooltip, int default_value, const Vector<const char *> &values,
	std::function<void(int)> on_change)
{
	auto label = WidgetLabel::create(name);
	label->setWidth(100);
	label->setToolTip(tooltip);
	grid->addChild(label, Gui::ALIGN_LEFT);

	auto combobox = WidgetComboBox::create();
	combobox->setToolTip(tooltip);
	for (const char *value : values)
		combobox->addItem(value);
	combobox->setCurrentItem(default_value);
	grid->addChild(combobox, Gui::ALIGN_EXPAND);
	grid->addChild(WidgetLabel::create(), Gui::ALIGN_LEFT);

	combobox->getEventChanged().connect(connections, [combobox, callback = std::move(on_change)]() {
		callback(combobox->getCurrentItem());
	});

	return combobox;
}

WidgetCheckBoxPtr add_bool_parameter(EventConnections &connections, const WidgetGridBoxPtr &grid,
	const char *name, const char *tooltip, bool default_value, std::function<void(bool)> on_change)
{
	auto label = WidgetLabel::create(name);
	label->setWidth(100);
	label->setToolTip(tooltip);
	grid->addChild(label, Gui::ALIGN_LEFT);

	auto checkbox = WidgetCheckBox::create();
	checkbox->setToolTip(tooltip);
	checkbox->setChecked(default_value);
	grid->addChild(checkbox, Gui::ALIGN_CENTER);
	grid->addChild(WidgetLabel::create(), Gui::ALIGN_LEFT);

	checkbox->getEventChanged().connect(connections, [checkbox, callback = std::move(on_change)](const WidgetPtr &) {
		callback(checkbox->isChecked());
	});

	return checkbox;
}

WidgetCheckBoxPtr add_bool_parameter(EventConnections &connections, const WidgetGroupBoxPtr &group,
	const char *name, const char *tooltip, bool default_value, std::function<void(bool)> on_change)
{
	auto hbox = WidgetHBox::create();
	group->addChild(hbox, Gui::ALIGN_EXPAND);

	auto label = WidgetLabel::create(name);
	label->setToolTip(tooltip);
	hbox->addChild(label, Gui::ALIGN_LEFT);

	auto checkbox = WidgetCheckBox::create();
	checkbox->setToolTip(tooltip);
	checkbox->setChecked(default_value);
	hbox->addChild(checkbox, Gui::ALIGN_RIGHT);

	checkbox->getEventChanged().connect(connections, [checkbox, callback = std::move(on_change)](const WidgetPtr &) {
		callback(checkbox->isChecked());
	});

	return checkbox;
}

WidgetButtonPtr add_button(EventConnections &connections, const WidgetPtr &parent,
	const char *name, const char *tooltip, std::function<void()> on_click)
{
	auto button = WidgetButton::create(name);
	button->setToolTip(tooltip);
	parent->addChild(button, Gui::ALIGN_EXPAND);

	button->getEventClicked().connect(connections, [callback = std::move(on_click)](const WidgetPtr &, int) {
		callback();
	});

	return button;
}

WidgetEditLinePtr add_string_field_with_button(EventConnections &connections, const WidgetGridBoxPtr &grid,
	const char *name, const char *tooltip, const char *default_value,
	const char *button_name, const char *button_tooltip, std::function<void()> on_click)
{
	auto label = WidgetLabel::create(name);
	label->setWidth(100);
	label->setToolTip(tooltip);
	grid->addChild(label, Gui::ALIGN_LEFT);

	auto editline = WidgetEditLine::create(default_value);
	editline->setToolTip(tooltip);
	grid->addChild(editline, Gui::ALIGN_EXPAND);

	auto button = WidgetButton::create(button_name);
	button->setToolTip(button_tooltip);
	grid->addChild(button, Gui::ALIGN_LEFT);

	button->getEventClicked().connect(connections, [callback = std::move(on_click)](const WidgetPtr &, int) {
		callback();
	});

	return editline;
}
