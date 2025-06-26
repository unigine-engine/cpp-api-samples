#include "SimpleInformationBox.h"
#include <UnigineWindowManager.h>

REGISTER_COMPONENT(SimpleInformationBox);

using namespace Unigine;
using namespace Math;

void SimpleInformationBox::init()
{
	window = WidgetWindow::create();
	WindowManager::getMainWindow()->addChild(window, Gui::ALIGN_OVERLAP);
	window->setMaxWidth(500);

	main_background = WidgetVBox::create();
	window->addChild(main_background, Gui::ALIGN_EXPAND);

	about_groupbox = WidgetGroupBox::create("About", 8, 8);
	main_background->addChild(about_groupbox, Gui::ALIGN_LEFT);

	about_label = WidgetLabel::create();
	about_label->setFontWrap(1);
	about_label->setFontRich(1);
	about_label->setWidth(600);
	about_groupbox->addChild(about_label, Gui::ALIGN_EXPAND);

	main_gridbox = WidgetGridBox::create(0);
	main_background->addChild(main_gridbox, Gui::ALIGN_EXPAND);

	window->arrange();

	previous_handle = Input::getMouseHandle();
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);
}

void SimpleInformationBox::shutdown()
{
	widget_connections.disconnectAll();
	window.deleteLater();
	Input::setMouseHandle(previous_handle);
}

void SimpleInformationBox::setAboutInfo(const char* str)
{
	about_label->setText(str);
	window->arrange();
}

void SimpleInformationBox::setParametersInfo(int index, const char *str)
{
	if (index < parameters_labels.size())
		parameters_labels[index]->setText(str);
	window->arrange();
}

WidgetPtr SimpleInformationBox::getColumn(int index)
{
	return main_gridbox->getChild(index);
}

void SimpleInformationBox::setColumnsCount(int count)
{
	while (main_gridbox->getNumChildren() > 0)
	{
		auto child = main_gridbox->getChild(0);
		main_gridbox->removeChild(child);
		child.deleteForce();
	}

	parameters_groupboxes.clear();
	parameters_labels.clear();
	additional_widgets.clear();

	main_gridbox->setNumColumns(count);

	for (int i = 0; i < count; i++)
	{
		auto background = WidgetVBox::create();
		main_gridbox->addChild(background, Gui::ALIGN_TOP);

		auto parameters_groupbox = WidgetGroupBox::create("Parameters", 8, 2);
		background->addChild(parameters_groupbox, Gui::ALIGN_LEFT);
		parameters_groupboxes.append(parameters_groupbox);

		auto parameters_label = WidgetLabel::create();
		parameters_label->setFontWrap(1);
		parameters_label->setFontRich(1);
		parameters_label->setWidth(width_in_pixels);
		parameters_groupbox->addChild(parameters_label, Gui::ALIGN_LEFT);
		parameters_labels.append(parameters_label);

		auto grid = WidgetGridBox::create(3);
		additional_widgets.append(grid);
		parameters_groupbox->addChild(grid, Gui::ALIGN_EXPAND);
	}

	window->arrange();
}

void SimpleInformationBox::setWidth(int width)
{
	if (width > 0)
	{
		width_in_pixels = width;
		about_label->setWidth(width_in_pixels*parameters_labels.size() + 1);
		for(auto& it : parameters_labels)
			it->setWidth(width_in_pixels);
	}
	window->arrange();
}

WidgetComboBoxPtr SimpleInformationBox::addCombobox(int index, const char* name, const char* tooltip)
{
	if (index >= parameters_groupboxes.size())
		return nullptr;

	auto combobox_container = additional_widgets[index];
	auto combobox_name_Label = WidgetLabel::create(name);
	combobox_container->addChild(combobox_name_Label, Gui::ALIGN_LEFT);
	auto combobox = WidgetComboBox::create();
	combobox_container->addChild(combobox, Gui::ALIGN_EXPAND);
	auto label_spacer = WidgetLabel::create();
	combobox_container->addChild(label_spacer);

	combobox_name_Label->setToolTip(tooltip);
	combobox->setToolTip(tooltip);


	return combobox;
}

Unigine::WidgetSliderPtr SimpleInformationBox::addSlider(int index, const char* name, float multiplier, const char* tooltip)
{
	if (index >= parameters_groupboxes.size())
		return nullptr;

	auto slider_container = additional_widgets[index];
	auto slider_name_Label = WidgetLabel::create(name);
	slider_container->addChild(slider_name_Label, Gui::ALIGN_LEFT);
	auto slider = WidgetSlider::create();
	slider_container->addChild(slider, Gui::ALIGN_EXPAND);
	auto slider_value_label = WidgetLabel::create();
	slider_value_label->setWidth(45);
	slider->getEventChanged().connect(widget_connections, [slider, slider_value_label, multiplier]() {
		slider_value_label->setText(String::format("%.2f", slider->getValue() * multiplier));
		});
	slider_value_label->setText(String::format("%.2f", slider->getValue() * multiplier));
	slider_container->addChild(slider_value_label, Gui::ALIGN_LEFT);

	slider_name_Label->setToolTip(tooltip);
	slider->setToolTip(tooltip);
	slider_value_label->setToolTip(tooltip);

	return slider;
}

void SimpleInformationBox::pushBackAboutInfo( const char* str, INFO_ALIGN align)
{

	String labelText = about_label->getText();

	switch (align)
	{
	case INFO_ALIGN::LEFT:
		labelText += "<p align=left>";
		break;
	case INFO_ALIGN::CENTER:
		labelText += "<p align=center>";
		break;
	case INFO_ALIGN::RIGHT:
		labelText += "<p align=right>";
		break;
	case INFO_ALIGN::JUSTIFY:
		labelText += "<p align=justify>";
		break;
	default:
		labelText += "<p>";
	}

	labelText += str;
	labelText += "</p>";

	about_label->setText(labelText);
	window->arrange();
}

void SimpleInformationBox::pushBackAboutInfo(const char* str1, const char* str2, INFO_ALIGN align)
{
	String labelText = about_label->getText();

	switch (align)
	{
	case INFO_ALIGN::LEFT:
		labelText += "<p align=left>";
		break;
	case INFO_ALIGN::CENTER:
		labelText += "<p align=center>";
		break;
	case INFO_ALIGN::RIGHT:
		labelText += "<p align=right>";
		break;
	case INFO_ALIGN::JUSTIFY:
		labelText += "<p align=justify>";
		break;
	default:
		labelText += "<p>";
	}

	labelText += str1;
	labelText += ":";
	labelText += "<right>";
	labelText += str2;
	labelText += "<left></p>";

	about_label->setText(labelText);
	window->arrange();
}

void SimpleInformationBox::pushBackParametersInfo(int index, const char* str, INFO_ALIGN align)
{
	if (index >= parameters_labels.size())
		return;

	String labelText = parameters_labels[index]->getText();

	switch (align)
	{
	case INFO_ALIGN::LEFT:
		labelText += "<p align=left>";
		break;
	case INFO_ALIGN::CENTER:
		labelText += "<p align=center>";
		break;
	case INFO_ALIGN::RIGHT:
		labelText += "<p align=right>";
		break;
	case INFO_ALIGN::JUSTIFY:
		labelText += "<p align=justify>";
		break;
	default:
		labelText += "<p>";
	}

	labelText += str;
	labelText += "</p>";

	parameters_labels[index]->setText(labelText);
	window->arrange();
}

void SimpleInformationBox::pushBackParametersInfo(int index, const char* str1, const char* str2, INFO_ALIGN align)
{
	if (index >= parameters_labels.size())
		return;

	String labelText = parameters_labels[index]->getText();

	switch (align)
	{
	case INFO_ALIGN::LEFT:
		labelText += "<p align=left>";
		break;
	case INFO_ALIGN::CENTER:
		labelText += "<p align=center>";
		break;
	case INFO_ALIGN::RIGHT:
		labelText += "<p align=right>";
		break;
	case INFO_ALIGN::JUSTIFY:
		labelText += "<p align=justify>";
		break;
	default:
		labelText += "<p>";
	}

	labelText += str1;
	labelText += ":";
	labelText += "<right>";
	labelText += str2;
	labelText += "<left></p>";

	parameters_labels[index]->setText(labelText);
	window->arrange();
}

void SimpleInformationBox::pushBackWhiteSpaceLineAboutInfo()
{
	String labelText = about_label->getText();
	labelText += "<br>";
	about_label->setText(labelText);
	window->arrange();
}

void SimpleInformationBox::pushBackWhiteSpaceLineParametersInfo(int index)
{
	if (index >= parameters_labels.size())
		return;

	String labelText = parameters_labels[index]->getText();
	labelText += "<br>";
	parameters_labels[index]->setText(labelText);
	window->arrange();
}

void SimpleInformationBox::showAdditionalWidgets(int index, bool value)
{
	if (index >= parameters_groupboxes.size())
		return;
	parameters_groupboxes[index]->setHidden(!value);
}
