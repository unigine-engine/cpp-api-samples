// Multi-joystick input sample with force feedback effect testing. Handles hot-plug
// events for dynamic connection/disconnection. Displays axes, POV switches, buttons.
// Creates UI for testing FFB effects: constant, friction, spring, wave patterns, etc.

#include "JoysticksInput.h"

#include "../../utils/Math.h"

#include <UnigineControls.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(JoysticksInput);

using namespace Unigine;
using namespace Math;

// Hot-plug events are subscribed and info structures are initialized for connected joysticks.
void JoysticksInput::init()
{
	description_window.createWindow();
	window_widget = description_window.getWindow();

	// Hot-plug events are subscribed for dynamic joystick connection/disconnection
	Input::getEventJoyConnected().connect(this, &JoysticksInput::on_joystick_connected);
	Input::getEventJoyDisconnected().connect(this, &JoysticksInput::on_joystick_disconnected);

	// Initialize info structures for all currently connected joysticks.
	// Each joystick gets its own FFB effect data storage.
	for (int i = 0; i < Input::getNumJoysticks(); i++)
	{

		joysticks_info.append({Input::getJoystick(i), "",
			Unigine::VectorStack<FFBEffectData>(static_cast<int>(Input::NUM_JOYSTICK_FORCE_FEEDBACKS))});
	}

	rebuild_joystick_uis();
}

// Per-joystick UI block is constructed: filter slider, live status label, optional FFB panel.
void JoysticksInput::build_joystick_ui(JoystickInfo &info)
{
	info.group = WidgetGroupBox::create(info.joystick->getName(), 8, 8);
	window_widget->addChild(info.group, Gui::ALIGN_LEFT);

	auto filter_row = WidgetHBox::create(5, 0);
	info.group->addChild(filter_row, Gui::ALIGN_EXPAND);

	auto filter_label = WidgetLabel::create("Filter");
	filter_label->setToolTip("Buttons sensitivity threshold");
	filter_label->setWidth(60);
	filter_row->addChild(filter_label);

	auto filter_slider = WidgetSlider::create(0, 100);
	filter_slider->setValue(ftoi(info.joystick->getFilter() * 100));
	filter_slider->setToolTip("Buttons sensitivity threshold");
	filter_row->addChild(filter_slider, Gui::ALIGN_EXPAND);

	InputJoystickPtr joystick = info.joystick;
	filter_slider->getEventChanged().connect(filter_connections, [filter_slider, joystick]() {
		joystick->setFilter(filter_slider->getValue() * 0.01f);
	});

	info.status_label = WidgetLabel::create();
	info.status_label->setFontRich(1);
	info.status_label->setFontWrap(1);
	info.group->addChild(info.status_label, Gui::ALIGN_EXPAND);

	create_ffb_ui(info, info.group);
}

// Force feedback UI controls are created for the specified joystick.
void JoysticksInput::create_ffb_ui(JoystickInfo &info, const WidgetPtr &container)
{
	// FFB UI is skipped if the joystick does not support any force feedback effects
	if (!is_ffb_supported(info.joystick))
		return;

	const auto ffb_container = WidgetGroupBox::create("Force Feedback Effects");
	const auto scroll = WidgetScrollBox::create();
	ffb_container->addChild(scroll);
	scroll->setHeight(400);
	scroll->setHScrollEnabled(false);
	scroll->setBorder(0);
	scroll->setPadding(10,10,10,10);
	scroll->setSpace(10,10);
	container->addChild(ffb_container, Gui::ALIGN_EXPAND);

	const auto &joystick = info.joystick;

	if (joystick->isForceFeedbackEffectSupported(Input::JOYSTICK_FORCE_FEEDBACK_CONSTANT))
		create_ffb_effect_ui("Constant", info, Input::JOYSTICK_FORCE_FEEDBACK_CONSTANT,
			true, false, false, scroll);

	if (joystick->isForceFeedbackEffectSupported(Input::JOYSTICK_FORCE_FEEDBACK_FRICTION))
		create_ffb_effect_ui("Friction", info, Input::JOYSTICK_FORCE_FEEDBACK_FRICTION,
			false, false, false, scroll);

	if (joystick->isForceFeedbackEffectSupported(Input::JOYSTICK_FORCE_FEEDBACK_DAMPER))
		create_ffb_effect_ui("Damper", info, Input::JOYSTICK_FORCE_FEEDBACK_DAMPER,
			false, false, false, scroll);

	if (joystick->isForceFeedbackEffectSupported(Input::JOYSTICK_FORCE_FEEDBACK_INERTIA))
		create_ffb_effect_ui("Inertia", info, Input::JOYSTICK_FORCE_FEEDBACK_INERTIA,
			false, false, false, scroll);

	if (joystick->isForceFeedbackEffectSupported(Input::JOYSTICK_FORCE_FEEDBACK_RAMP))
		create_ffb_effect_ui("Ramp", info, Input::JOYSTICK_FORCE_FEEDBACK_RAMP, true,
			true, false, scroll);

	if (joystick->isForceFeedbackEffectSupported(Input::JOYSTICK_FORCE_FEEDBACK_SPRING))
		create_ffb_effect_ui("Spring", info, Input::JOYSTICK_FORCE_FEEDBACK_SPRING,
			false, false, false, scroll);

	if (joystick->isForceFeedbackEffectSupported(Input::JOYSTICK_FORCE_FEEDBACK_SINEWAVE))
		create_ffb_effect_ui("Sine Wave", info,
			Input::JOYSTICK_FORCE_FEEDBACK_SINEWAVE, false, false, true, scroll);

	if (joystick->isForceFeedbackEffectSupported(Input::JOYSTICK_FORCE_FEEDBACK_SQUAREWAVE))
		create_ffb_effect_ui("Square Wave", info,
			Input::JOYSTICK_FORCE_FEEDBACK_SQUAREWAVE, false, false, true, scroll);

	if (joystick->isForceFeedbackEffectSupported(Input::JOYSTICK_FORCE_FEEDBACK_TRIANGLEWAVE))
		create_ffb_effect_ui("Triangle Wave", info,
			Input::JOYSTICK_FORCE_FEEDBACK_TRIANGLEWAVE, false, false, true, scroll);

	if (joystick->isForceFeedbackEffectSupported(Input::JOYSTICK_FORCE_FEEDBACK_SAWTOOTHUPWAVE))
		create_ffb_effect_ui("Saw Tooth Up", info,
			Input::JOYSTICK_FORCE_FEEDBACK_SAWTOOTHUPWAVE, false, false, true, scroll);

	if (joystick->isForceFeedbackEffectSupported(Input::JOYSTICK_FORCE_FEEDBACK_SAWTOOTHDOWNWAVE))
		create_ffb_effect_ui("Saw Tooth Down", info,
			Input::JOYSTICK_FORCE_FEEDBACK_SAWTOOTHDOWNWAVE, false, false, true, scroll);
}

// Existing per-joystick groupboxes are destroyed and event connections reset.
void JoysticksInput::clear_joystick_uis()
{
	filter_connections.disconnectAll();
	ffb_connections.disconnectAll();

	for (auto &info : joysticks_info)
	{
		if (info.group)
		{
			info.group.deleteLater();
			info.group.clear();
		}
		info.status_label.clear();
		info.ramp_effect_data = {};
	}
}

// All joystick UI blocks are rebuilt from the current joysticks_info list.
void JoysticksInput::rebuild_joystick_uis()
{
	clear_joystick_uis();

	for (auto &info : joysticks_info)
	{
		if (!info.joystick || !info.joystick->isAvailable())
			continue;

		build_joystick_ui(info);
	}
}

// Live joystick state is refreshed each frame in the status labels.
void JoysticksInput::update()
{
	for (auto &info : joysticks_info)
	{
		if (!info.joystick || !info.joystick->isAvailable() || !info.status_label)
			continue;

		const auto &joystick = info.joystick;

		int num_buttons = joystick->getNumButtons();
		for (int j = 0; j < num_buttons; j++)
			if (joystick->isButtonPressed(j) != 0)
				info.last_pressed_button = joystick->getButtonName(j);

		String text;
		text += String::format("Number: %d\n\n", joystick->getNumber());

		int num_axes = joystick->getNumAxes();
		text += String::format("<b>Axes %d</b>\n", num_axes);
		for (int j = 0; j < num_axes; j++)
			text += String::format("%s: %.2f\n", joystick->getAxisName(j), joystick->getAxis(j));
		text += "\n";

		int num_povs = joystick->getNumPovs();
		text += String::format("<b>POVs %d</b>\n", num_povs);
		for (int j = 0; j < num_povs; j++)
			text += String::format("%s: %d\n", joystick->getPovName(j), (int)joystick->getPov(j));
		text += "\n";

		text += String::format("<b>Buttons Count: %d</b>\n", num_buttons);
		text += String::format("Last Pressed Button: %s\n\n", info.last_pressed_button.get());

		text += String::format("FFB Status: %s",
			is_ffb_supported(joystick) ? "Supported" : "Unsupported");

		info.status_label->setText(text.get());
	}

	// Ramp effect timers are updated to auto-toggle buttons when duration expires
	for (auto &info : joysticks_info)
		info.ramp_effect_data.update();
}

// Event connections are released and joystick info is cleared.
void JoysticksInput::shutdown()
{
	clear_joystick_uis();
	joysticks_info.clear();
	description_window.shutdown();
}

// New joystick info structure is created when a joystick is hot-plugged.
void JoysticksInput::on_joystick_connected(int num)
{
	joysticks_info.push_back({Input::getJoystick(num), "",
		Unigine::VectorStack<FFBEffectData>(
			static_cast<int>(Input::NUM_JOYSTICK_FORCE_FEEDBACKS))});

	rebuild_joystick_uis();
}

// Joystick info is removed when a joystick is disconnected.
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

	rebuild_joystick_uis();
}

// UI panel for a single FFB effect is created with sliders and play/stop button.
WidgetPtr JoysticksInput::create_ffb_effect_ui(const String &name, JoystickInfo &info,
	Input::JOYSTICK_FORCE_FEEDBACK_EFFECT effect_type, bool need_magnitude, bool need_duration,
	bool need_frequency, const WidgetPtr &parent)
{
	const auto play_button = WidgetButton::create("Play");

	const auto create_labeled_slider_float = [this](
			const String &name,
			const WidgetPtr &slider_parent,
			float initial_value,
			float min,
			float max,
			const std::function<void(float)> &on_change
	) {
		on_change(initial_value);

		const auto name_label = WidgetLabel::create(name);
		name_label->setWidth(100);

		int slider_min = 0;
		int slider_max = 1000;

		auto slider = WidgetSlider::create(slider_min, slider_max);
		slider->setValue(ftoi(Utils::mapRange(initial_value, min, max, slider_min, slider_max)));
		slider->setWidth(150);

		auto input = WidgetEditLine::create();
		input->setWidth(40);
		input->setText(String::format("%.2f", initial_value));
		input->setValidator(Gui::VALIDATOR_FLOAT);

		input->getEventChanged().connect(ffb_connections, [slider, min, max, slider_min, slider_max, on_change](const WidgetPtr &w) {
			auto line = static_ptr_cast<WidgetEditLine>(w);
			float value = String::atof(line->getText());

			if (value < min || value > max)
			{
				value = clamp(value, min, max);
				line->getEventChanged().setEnabled(false);
				line->setText(String::format("%.2f", value));
				line->getEventChanged().setEnabled(true);
			}

			slider->getEventChanged().setEnabled(false);
			slider->setValue(ftoi(Utils::mapRange(value, min, max, slider_min, slider_max)));
			slider->getEventChanged().setEnabled(true);
			on_change(value);
		});

		slider->getEventChanged().connect(ffb_connections, [slider, input, min, max, slider_min, slider_max, on_change] {
			const auto value = Utils::mapRange(float(slider->getValue()), slider_min, slider_max, min, max);
			input->getEventChanged().setEnabled(false);
			input->setText(String::format("%.2f", value));
			input->getEventChanged().setEnabled(true);
			on_change(value);
		});

		slider_parent->addChild(name_label);
		slider_parent->addChild(slider);
		slider_parent->addChild(input, Gui::ALIGN_RIGHT);
	};
	auto container = WidgetGroupBox::create(name);
	container->setSpace(5,5);
	parent->addChild(container);
	const auto grid = WidgetGridBox::create(3);
	grid->setSpace(5, 5);
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

// FFB effect is dispatched to the appropriate joystick method based on effect type.
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

// Active FFB effect is stopped on the joystick.
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

// All FFB effect types are checked to determine if joystick supports any.
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

// Duration timer is updated; play button is auto-toggled off when effect expires.
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
