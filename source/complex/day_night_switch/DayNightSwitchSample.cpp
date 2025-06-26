#pragma once
#include "DayNightSwitchSample.h"
#include <UnigineVisualizer.h>
#include "../../utils/Utils.h"

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(DayNightSwitchSample);


void DayNightSwitchSample::init()
{
	sun = getComponent<SunController>(sun_node.get());
	if (!sun)
	{
		Log::error("DayNightSwitchSample::init find SunController component from sun_node!\n");
	}

	switcher = getComponent<DayNightSwitcher>(switcher_node.get());
	if (!switcher)
	{
		Log::error("DayNightSwitchSample::init cannot get DayNightSwitcher component from switcher_node!\n");
	}

	Visualizer::setEnabled(true);
	const int max_minutes = 24 * 60;
	const int noon_in_minutes = 12 * 60;

	window.createWindow();

	WidgetGroupBoxPtr params = window.getParameterGroupBox();
	WidgetPtr grid = params->getChild(0);

	auto label = WidgetLabel::create("Global time:");
	label->setWidth(100);
	grid->addChild(label, Gui::ALIGN_LEFT);

	time_slider = WidgetSlider::create();
	time_slider->setMinValue(0);
	time_slider->setMaxValue(max_minutes);

	time_slider->setWidth(200);
	time_slider->setButtonWidth(20);
	time_slider->setButtonHeight(20);
	time_slider->setToolTip("Global time");
	grid->addChild(time_slider, Gui::ALIGN_LEFT);

	time_label = WidgetLabel::create("00:00");
	time_label->setWidth(20);
	grid->addChild(time_label, Gui::ALIGN_LEFT);

	time_slider->getEventChanged().connect(*this, [this]() {
		int time = time_slider->getValue();
		String time_string = getTimeString(time);
		time_label->setText(time_string);
		sun->setTime(time * 60);
		});

	time_slider->setValue(noon_in_minutes);

	//========== Threshold Slider=========//
	WidgetSliderPtr threshold_slider = window.addFloatParameter("Zenith angle threshold", "Zenith angle threshold", switcher->getZenithThreshold(), 70.0f, 120.0f, [this](int v) {
		switcher->setZenithThreshold(v);
		});

	//========== Morning Slider=========//
	auto morning_label = WidgetLabel::create("Morning time bound:");
	morning_label->setWidth(100);
	grid->addChild(morning_label, Gui::ALIGN_LEFT);

	auto morning_slider = WidgetSlider::create();
	morning_slider->setMinValue(0);
	morning_slider->setMaxValue(max_minutes);

	morning_slider->setWidth(200);
	morning_slider->setButtonWidth(20);
	morning_slider->setButtonHeight(20);
	grid->addChild(morning_slider, Gui::ALIGN_LEFT);

	auto morning_time_label = WidgetLabel::create("00:00");
	morning_time_label->setWidth(20);
	grid->addChild(morning_time_label, Gui::ALIGN_LEFT);
	int time = switcher->getControlMorningTime();

	morning_slider->getEventChanged().connect(*this, [this, morning_slider, morning_time_label]() {
		int time = morning_slider->getValue();
		String time_string = getTimeString(time);
		morning_time_label->setText(time_string);
		int hours = time / 60;
		int minutes = time % 60;
		switcher->setControlMorningTime(ivec2(hours, minutes));
		});
	morning_slider->setValue(switcher->getControlMorningTime());

	//========== Evening Slider =========//
	auto evening_label = WidgetLabel::create("Evening time bound:");
	evening_label->setWidth(100);
	grid->addChild(evening_label, Gui::ALIGN_LEFT);

	auto evening_slider = WidgetSlider::create();
	evening_slider->setMinValue(0);
	evening_slider->setMaxValue(max_minutes);

	evening_slider->setWidth(200);
	evening_slider->setButtonWidth(20);
	evening_slider->setButtonHeight(20);
	evening_slider->setToolTip("Evening time bound");
	grid->addChild(evening_slider, Gui::ALIGN_LEFT);

	auto evening_time_label = WidgetLabel::create("00:00");
	evening_time_label->setWidth(20);
	grid->addChild(evening_time_label, Gui::ALIGN_LEFT);

	evening_slider->getEventChanged().connect(*this, [this, evening_slider, evening_time_label]() {
		int time = evening_slider->getValue();
		String time_string = getTimeString(time);
		evening_time_label->setText(time_string);
		int hours = time / 60;
		int minutes = time % 60;
		switcher->setControlEveningTime(ivec2(hours, minutes));
		});
	evening_slider->setValue(switcher->getControlEveningTime());



	WidgetComboBoxPtr control_type = WidgetComboBox::create();
	params->addChild(control_type, Gui::ALIGN_LEFT);
	control_type->addItem("Zenith angle threshold");
	control_type->addItem("Time");
	control_type->getEventChanged().connect(*this, [this, control_type, threshold_slider, grid]() {
		int item = control_type->getCurrentItem();
		auto type = static_cast<DayNightSwitcher::CONTROL_TYPE>(item);
		switcher->setControlType(type);
		});

	//========== Timescale slider =========//

	int min_value = 0;
	int max_value = 60 * 60 * 12;
	float default_Value = sun->getTimescale();
	WidgetSliderPtr timescale_slider = window.addIntParameter("Timescale", "Timescale", default_Value, min_value, max_value, [this](int v) {
		sun->setTimescale(v);
		});

	//========== Control type Switch =========//
	WidgetCheckBoxPtr continuous_check_box = WidgetCheckBox::create("Continuous sun rotation:");
	params->addChild(continuous_check_box, Gui::ALIGN_LEFT);
	continuous_check_box->getEventChanged().connect(*this, [this, continuous_check_box](const WidgetPtr& checkbox) {
		sun->setContinuousEnabled(continuous_check_box->isChecked());
		});
	continuous_check_box->setChecked(sun->isContinuous());


	sun->getEventOnTimeChanged().connect(*this, [this](float time) {// updating slider to current time coming from sun controller
		MUTE_EVENT(sun->getEventOnTimeChanged());
		time_slider->setValue(time / 60);
		});
}

void DayNightSwitchSample::shutdown()
{
	Visualizer::setEnabled(false);
	window.shutdown();
}

void DayNightSwitchSample::update()
{
	Visualizer::renderVector(Vec3(0, 0, 2), Vec3(0, 0, 7), vec4_red,0.5f);
	Visualizer::renderVector(Vec3(0, 0, 2), Vec3(0, 0, 2) + static_cast<Vec3>(sun_node->getWorldDirection(AXIS_Z)) * 5, vec4_blue,0.5f);
}

String DayNightSwitchSample::getTimeString(int minutes)
{
	int hours = minutes / 60;
	int left_minutes = minutes % 60;

	StringStack<> str = "";
	if (hours < 10)
	{
		str += "0";
	}
	str += String::itoa(hours);
	str += ":";
	if (left_minutes < 10)
	{
		str += "0";
	}
	str += String::itoa(left_minutes);
	return str;
}
