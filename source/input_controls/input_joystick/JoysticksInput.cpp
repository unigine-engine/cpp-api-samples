#include "JoysticksInput.h"

#include "../../utils/Math.h"
#include "../../utils/SimpleInformationBox.h"

#include <UnigineControls.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(JoysticksInput);

using namespace Unigine;
using namespace Math;

void JoysticksInput::init()
{
	Input::getEventJoyConnected().connect(event_connections, this,
		&JoysticksInput::on_joystick_connected);
	Input::getEventJoyDisconnected().connect(event_connections, this,
		&JoysticksInput::on_joystick_disconnected);

	for (int i = 0; i < Input::getNumJoysticks(); i++)
	{

		joysticks_info.append({Input::getJoystick(i), "",
			Unigine::VectorStack<FFBEffectData>(static_cast<int>(Input::NUM_JOYSTICK_FORCE_FEEDBACKS))});
	}

	info = getComponent<SimpleInformationBox>(node);

	info->setWindowTitle("Joystick Controls Sample");
	info->setWidth(300);
	info->pushBackAboutInfo("This sample demostrates the simple usage of multiple Joystick input.");
}

void JoysticksInput::create_ffb_ui(int info_column, Unigine::InputJoystickPtr &joystick)
{

	if (!is_ffb_supported(joystick))
		return;

	const auto ffb_container = WidgetGroupBox::create("Force Feedback Effects");
	const auto scroll = WidgetScrollBox::create();
	ffb_container->addChild(scroll);
	// scroll->setWidth(350);
	scroll->setHeight(400);
	scroll->setHScrollEnabled(false);
	scroll->setBorder(0);
	scroll->setPadding(10,10,10,10);
	scroll->setSpace(10,10);
	const auto container = info->getColumn(info_column);
	container->addChild(ffb_container, Gui::ALIGN_EXPAND);

	if (joystick->isForceFeedbackEffectSupported(Input::JOYSTICK_FORCE_FEEDBACK_CONSTANT))
		create_ffb_effect_ui("Constant", joysticks_info[info_column], Input::JOYSTICK_FORCE_FEEDBACK_CONSTANT,
			true, false, false, scroll);

	if (joystick->isForceFeedbackEffectSupported(Input::JOYSTICK_FORCE_FEEDBACK_FRICTION))
		create_ffb_effect_ui("Friction", joysticks_info[info_column], Input::JOYSTICK_FORCE_FEEDBACK_FRICTION,
			false, false, false, scroll);

	if (joystick->isForceFeedbackEffectSupported(Input::JOYSTICK_FORCE_FEEDBACK_DAMPER))
		create_ffb_effect_ui("Damper", joysticks_info[info_column], Input::JOYSTICK_FORCE_FEEDBACK_DAMPER,
			false, false, false, scroll);

	if (joystick->isForceFeedbackEffectSupported(Input::JOYSTICK_FORCE_FEEDBACK_INERTIA))
		create_ffb_effect_ui("Inertia", joysticks_info[info_column], Input::JOYSTICK_FORCE_FEEDBACK_INERTIA,
			false, false, false, scroll);

	if (joystick->isForceFeedbackEffectSupported(Input::JOYSTICK_FORCE_FEEDBACK_RAMP))
		create_ffb_effect_ui("Ramp", joysticks_info[info_column], Input::JOYSTICK_FORCE_FEEDBACK_RAMP, true,
			true, false, scroll);

	if (joystick->isForceFeedbackEffectSupported(Input::JOYSTICK_FORCE_FEEDBACK_SPRING))
		create_ffb_effect_ui("Spring", joysticks_info[info_column], Input::JOYSTICK_FORCE_FEEDBACK_SPRING,
			false, false, false, scroll);

	if (joystick->isForceFeedbackEffectSupported(Input::JOYSTICK_FORCE_FEEDBACK_SINEWAVE))
		create_ffb_effect_ui("Sine Wave", joysticks_info[info_column],
			Input::JOYSTICK_FORCE_FEEDBACK_SINEWAVE, false, false, true, scroll);

	if (joystick->isForceFeedbackEffectSupported(Input::JOYSTICK_FORCE_FEEDBACK_SQUAREWAVE))
		create_ffb_effect_ui("Square Wave", joysticks_info[info_column],
			Input::JOYSTICK_FORCE_FEEDBACK_SQUAREWAVE, false, false, true, scroll);

	if (joystick->isForceFeedbackEffectSupported(Input::JOYSTICK_FORCE_FEEDBACK_TRIANGLEWAVE))
		create_ffb_effect_ui("Triangle Wave", joysticks_info[info_column],
			Input::JOYSTICK_FORCE_FEEDBACK_TRIANGLEWAVE, false, false, true, scroll);

	if (joystick->isForceFeedbackEffectSupported(Input::JOYSTICK_FORCE_FEEDBACK_SAWTOOTHUPWAVE))
		create_ffb_effect_ui("Saw Tooth Up", joysticks_info[info_column],
			Input::JOYSTICK_FORCE_FEEDBACK_SAWTOOTHUPWAVE, false, false, true, scroll);

	if (joystick->isForceFeedbackEffectSupported(Input::JOYSTICK_FORCE_FEEDBACK_SAWTOOTHDOWNWAVE))
		create_ffb_effect_ui("Saw Tooth Down", joysticks_info[info_column],
			Input::JOYSTICK_FORCE_FEEDBACK_SAWTOOTHDOWNWAVE, false, false, true, scroll);
}
void JoysticksInput::update()
{
	int joysticks_count = 0;
	for (const auto &info : joysticks_info)
	{
		if (info.joystick && info.joystick->isAvailable())
		{
			++joysticks_count;
		}
	}

	if (info->getColumnsCount() != joysticks_count)
	{
		info->setColumnsCount(joysticks_count);
		for (int i = 0; i < joysticks_info.size(); i++)
		{
			if (!joysticks_info[i].joystick && joysticks_info[i].joystick->isAvailable())
				continue;

			auto &joystick = joysticks_info[i].joystick;

			auto filter = info->addSlider(i, "Filter", 0.01f, "Buttons sensitivity threshold");
			filter->setValue(joysticks_info[i].joystick->getFilter() * 100);
			filter->getEventChanged().connect(filter_connections, [this, filter, i]() {
				joysticks_info[i].joystick->setFilter(filter->getValue() * 0.01f);
			});

			create_ffb_ui(i, joystick);
		}
	}

	for (int i = 0, column_index = 0; i < joysticks_info.size(); i++)
	{
		if (!joysticks_info[i].joystick && joysticks_info[i].joystick->isAvailable())
			continue;

		// getting information about the joystick
		const auto &joystick = joysticks_info[i].joystick;

		String name = joystick->getName();
		int number = joystick->getNumber();

		int num_axes = joystick->getNumAxes();
		Vector<String> axis_names;
		Vector<float> axis_values;
		for (int j = 0; j < num_axes; j++)
		{
			axis_names.push_back(joystick->getAxisName(j));
			axis_values.push_back(joystick->getAxis(j));
		}

		int num_povs = joystick->getNumPovs();
		Vector<String> pov_names;
		Vector<Input::JOYSTICK_POV> pov_values;
		for (int j = 0; j < num_povs; j++)
		{
			pov_names.push_back(joystick->getPovName(j));
			pov_values.push_back(joystick->getPov(j));
		}

		int num_buttons = joystick->getNumButtons();
		for (int j = 0; j < num_buttons; j++)
			if (joystick->isButtonPressed(j) != 0)
				joysticks_info[i].last_pressed_button = joystick->getButtonName(j);

		// filling information about the joystick
		info->clearParametersInfo(column_index);

		info->pushBackParametersInfo(column_index, name.get(),
			SimpleInformationBox::INFO_ALIGN::CENTER);
		info->pushBackParametersInfo(column_index, "Number", String::itoa(number),
			SimpleInformationBox::INFO_ALIGN::LEFT);
		info->pushBackWhiteSpaceLineParametersInfo(column_index);

		info->pushBackParametersInfo(column_index, "Axes " + String::itoa(num_axes),
			SimpleInformationBox::INFO_ALIGN::CENTER);
		for (int j = 0; j < num_axes; j++)
			info->pushBackParametersInfo(column_index, axis_names[j], String::format("%.2f",axis_values[j]));
		info->pushBackWhiteSpaceLineParametersInfo(column_index);

		info->pushBackParametersInfo(column_index, "POVs " + String::itoa(num_povs),
			SimpleInformationBox::INFO_ALIGN::CENTER);
		for (int j = 0; j < num_povs; j++)
			info->pushBackParametersInfo(column_index, pov_names[j], String::itoa(pov_values[j]));
		info->pushBackWhiteSpaceLineParametersInfo(column_index);

		info->pushBackParametersInfo(column_index, "Buttons Count", String::itoa(num_buttons),
			SimpleInformationBox::INFO_ALIGN::CENTER);
		info->pushBackWhiteSpaceLineParametersInfo(column_index);

		info->pushBackParametersInfo(column_index, "Last Pressed Button",
			joysticks_info[i].last_pressed_button);
		info->pushBackParametersInfo(column_index, "                   ",
			SimpleInformationBox::INFO_ALIGN::CENTER);
		info->pushBackWhiteSpaceLineParametersInfo(column_index);

		String ffb_status = is_ffb_supported(joystick) ? "Supported" : "Unsupported";
		info->pushBackParametersInfo(column_index, "FFB Status", ffb_status,
			SimpleInformationBox::INFO_ALIGN::LEFT);
		info->pushBackParametersInfo(column_index, "                   ",
			SimpleInformationBox::INFO_ALIGN::RIGHT);
		info->pushBackWhiteSpaceLineParametersInfo(column_index);


		++column_index;
	}

	for (auto& info : joysticks_info)
		info.ramp_effect_data.update();
}

void JoysticksInput::shutdown()
{
	filter_connections.disconnectAll();
	joysticks_info.clear();
}

void JoysticksInput::on_joystick_connected(int num)
{
	joysticks_info.push_back({Input::getJoystick(num), "",
		Unigine::VectorStack<FFBEffectData>(
			static_cast<int>(Input::NUM_JOYSTICK_FORCE_FEEDBACKS))});
}

void JoysticksInput::on_joystick_disconnected(int num)
{
	for (int i = 0; i < joysticks_info.size(); i++)
	{
		if (joysticks_info[i].joystick && joysticks_info[i].joystick->getNumber() == num)
		{
			joysticks_info.remove(i);
			break;
		}
	}
}
WidgetPtr JoysticksInput::create_ffb_effect_ui(const String &name, JoystickInfo &info,
	Input::JOYSTICK_FORCE_FEEDBACK_EFFECT effect_type, bool need_magnitude, bool need_duration,
	bool need_frequency, const WidgetPtr &parent)
{
	const auto play_button = WidgetButton::create("Play");

	const auto create_labeled_slider_float = [this](const String &name, const WidgetPtr &slider_parent,
												 float initial_value, float min, float max,
												 const std::function<void(float)> &on_change) {
		const auto name_label = WidgetLabel::create(name);
		name_label->setWidth(100);
		slider_parent->addChild(name_label);
		auto slider = WidgetSlider::create();
		slider_parent->addChild(slider);

		slider->setMinValue(0);
		slider->setMaxValue(1000);
		slider->setValue(ftoi(Utils::mapRange(initial_value, min, max, 0.f, 1000.f)));

		slider->setWidth(150);
		auto label = WidgetLabel::create(String::format("%.2f", initial_value));
		label->setWidth(40);
		slider_parent->addChild(label, Gui::ALIGN_RIGHT);
		on_change(initial_value);

		slider->getEventChanged().connect(ffb_connections, [slider, label, min, max, on_change] {
			const auto value = Utils::mapRange(slider->getValue(), 0, 1000, min, max);
			label->setText(String::format("%.2f", value));
			on_change(value);
		});
	};
	auto container = WidgetGroupBox::create(name);
	container->setSpace(5,5);
	parent->addChild(container);
	const auto grid = WidgetGridBox::create(3);
	container->addChild(grid);

	create_labeled_slider_float("Force", grid, 0.f, 0.f, 1.f,
		[effect_type, &info, play_button, this, need_duration](float value) {
			info.ffb_effect_data[effect_type].force = value;

			if (!info.joystick || !info.joystick->isAvailable())
				return;
			if (!info.joystick->isForceFeedbackEffectSupported(effect_type))
				return;

			if (!play_button->isToggled())
				return;

			if (need_duration)
			{
				info.ramp_effect_data.desired_time = info.ffb_effect_data[effect_type].duration
					/ 1000000.f;
				info.ramp_effect_data.current_time = 0;
			}
			play_ffb_effect(info.ffb_effect_data[effect_type], effect_type, info.joystick);
		});

	if (need_magnitude)
	{
		create_labeled_slider_float("Magnitude", grid, 0.f, -1.f, 1.f,
			[effect_type, &info, play_button, need_duration, this](float value) {
				info.ffb_effect_data[effect_type].magnitude = value;

				if (!info.joystick || !info.joystick->isAvailable())
					return;
				if (!info.joystick->isForceFeedbackEffectSupported(effect_type))
					return;

				if (!play_button->isToggled())
					return;

				if (need_duration)
				{
					info.ramp_effect_data.desired_time = info.ffb_effect_data[effect_type].duration
						/ 1000000.f;
					info.ramp_effect_data.current_time = 0;
				}
				play_ffb_effect(info.ffb_effect_data[effect_type], effect_type, info.joystick);
			});
	}

	if (need_duration)
	{
		create_labeled_slider_float("Duration", grid, 0.f, 0.f, 10.f,
			[effect_type, &info, play_button, need_duration](float value) {
				info.ffb_effect_data[effect_type].duration = static_cast<unsigned long long>(
					value * 1000000.f);

				if (!info.joystick || !info.joystick->isAvailable())
					return;
				if (!info.joystick->isForceFeedbackEffectSupported(effect_type))
					return;

				if (!play_button->isToggled())
					return;

				if (need_duration)
				{
					info.ramp_effect_data.desired_time = info.ffb_effect_data[effect_type].duration
						/ 1000000.f;
					info.ramp_effect_data.current_time = 0;
				}
				play_ffb_effect(info.ffb_effect_data[effect_type], effect_type, info.joystick);
			});
	}

	if (need_frequency)
	{
		create_labeled_slider_float("Frequency", grid, 0.f, 0.f, 10.f,
			[effect_type, &info, play_button](float value) {
				info.ffb_effect_data[effect_type].frequency = value;

				if (!info.joystick || !info.joystick->isAvailable())
					return;
				if (!info.joystick->isForceFeedbackEffectSupported(effect_type))
					return;

				if (!play_button->isToggled())
					return;

				play_ffb_effect(info.ffb_effect_data[effect_type], effect_type, info.joystick);
			});
	}

	container->addChild(play_button, Gui::ALIGN_EXPAND);

	play_button->setToggleable(true);
	play_button->getEventClicked().connect(ffb_connections,
		[this, &info, effect_type, play_button, need_duration] {
			if (!play_button->isToggled())
			{
				if (!info.joystick->isForceFeedbackEffectPlaying(effect_type))
				{
					Log::error("JoystickInput::create_ffb_effect_ui(): effect is not playing\n");
				}
				stop_ffb_effect(effect_type, info.joystick);
			}
			else
			{
				if (info.joystick->isForceFeedbackEffectPlaying(effect_type))
				{
					Log::error("JoystickInput::create_ffb_effect_ui(): effect is playing\n");
				}
				if (need_duration)
				{
					info.ramp_effect_data.button = play_button;
					info.ramp_effect_data.desired_time = info.ffb_effect_data[effect_type].duration
						/ 1000000.f;
					info.ramp_effect_data.current_time = 0;
				}
				play_ffb_effect(info.ffb_effect_data[effect_type], effect_type, info.joystick);
			}
		});

	return container;
}


void JoysticksInput::play_ffb_effect(const FFBEffectData &data,
	Input::JOYSTICK_FORCE_FEEDBACK_EFFECT effect_type, const InputJoystickPtr &joystick)
{
	if (!joystick || !joystick->isAvailable())
		return;

	switch (effect_type)
	{
	case Input::JOYSTICK_FORCE_FEEDBACK_CONSTANT:
	{
		joystick->playForceFeedbackEffectConstant(data.force);
		break;
	}
	case Input::JOYSTICK_FORCE_FEEDBACK_FRICTION:
	{
		joystick->playForceFeedbackEffectFriction(data.force);
		break;
	}
	case Input::JOYSTICK_FORCE_FEEDBACK_DAMPER:
	{
		joystick->playForceFeedbackEffectDamper(data.force);
		break;
	}
	case Input::JOYSTICK_FORCE_FEEDBACK_INERTIA:
	{
		joystick->playForceFeedbackEffectInertia(data.force);
		break;
	}
	case Input::JOYSTICK_FORCE_FEEDBACK_RAMP:
	{
		joystick->playForceFeedbackEffectRamp(data.force, data.duration);
		break;
	}
	case Input::JOYSTICK_FORCE_FEEDBACK_SPRING:
	{
		joystick->playForceFeedbackEffectSpring(data.force);
		break;
	}
	case Input::JOYSTICK_FORCE_FEEDBACK_SINEWAVE:
	{
		joystick->playForceFeedbackEffectSineWave(data.force, data.frequency);
		break;
	}
	case Input::JOYSTICK_FORCE_FEEDBACK_SQUAREWAVE:
	{
		joystick->playForceFeedbackEffectSquareWave(data.force, data.frequency);
		break;
	}
	case Input::JOYSTICK_FORCE_FEEDBACK_TRIANGLEWAVE:
	{
		joystick->playForceFeedbackEffectTriangleWave(data.force, data.frequency);
		break;
	}
	case Input::JOYSTICK_FORCE_FEEDBACK_SAWTOOTHDOWNWAVE:
	{
		joystick->playForceFeedbackEffectSawtoothDownWave(data.force, data.frequency);
		break;
	}
	case Input::JOYSTICK_FORCE_FEEDBACK_SAWTOOTHUPWAVE:
	{
		joystick->playForceFeedbackEffectSawtoothUpWave(data.force, data.frequency);
		break;
	}
	default:
	{
		return;
	}
	}
}

void JoysticksInput::stop_ffb_effect(Input::JOYSTICK_FORCE_FEEDBACK_EFFECT effect_type,
	const InputJoystickPtr &joystick)
{
	if (joystick->isForceFeedbackEffectSupported(effect_type)
		&& joystick->isForceFeedbackEffectPlaying(effect_type))
	{
		joystick->stopForceFeedbackEffect(effect_type);
	}
	else
	{
		Log::error("JoysticksInput::stop_ffb_effect: couldn't stop ffb effect");
	}
}

bool JoysticksInput::is_ffb_supported(const InputJoystickPtr &joystick)
{
	for (int i = 0; i < static_cast<int>(Input::NUM_JOYSTICK_FORCE_FEEDBACKS); ++i)
	{
		auto effect_type = static_cast<Input::JOYSTICK_FORCE_FEEDBACK_EFFECT>(i);
		if (joystick->isForceFeedbackEffectSupported(effect_type))
		{
			return true;
		}
	}
	return false;
}

void JoysticksInput::DurationEffectData::update()
{
	if (!button)
		return;

	if (current_time >= desired_time)
		return;

	current_time += Game::getIFps();
	if (current_time >= desired_time)
	{
		button->setToggled(false);
	}
}
