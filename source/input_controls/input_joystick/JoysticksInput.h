#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineInput.h>

class SimpleInformationBox;

/// The implementation of force feedback effects (FFB) in this sample is managed using the
/// play_ffb_effect method. This method is responsible for executing the desired FFB effect on the
/// connected joystick based on the parameters provided. It ensures compatibility with different
/// types of effects and properly invokes the corresponding force feedback functions defined in the
/// InputJoystick class.

class JoysticksInput: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(JoysticksInput, Unigine::ComponentBase);

	COMPONENT_INIT(init, 1);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);
private:
	struct FFBEffectData;

	static void play_ffb_effect(const FFBEffectData &data,
		Unigine::Input::JOYSTICK_FORCE_FEEDBACK_EFFECT effect_type,
		const Unigine::InputJoystickPtr &joystick);
	static void stop_ffb_effect(Unigine::Input::JOYSTICK_FORCE_FEEDBACK_EFFECT effect_type,
		const Unigine::InputJoystickPtr &joystick);

private:
	void init();
	void update();
	void shutdown();

	void on_joystick_connected(int num);
	void on_joystick_disconnected(int num);

	Unigine::EventConnections event_connections;
	Unigine::EventConnections filter_connections;
	Unigine::EventConnections ffb_connections;

	struct DurationEffectData
	{
		Unigine::WidgetButtonPtr button;
		float desired_time{0.f};
		float current_time{0.f};
		void update();
	};

	struct FFBEffectData
	{
		Unigine::Input::JOYSTICK_FORCE_FEEDBACK_EFFECT type{};
		float magnitude{0.f};
		float force{0.f};
		unsigned long long duration{0};
		float frequency{0.f};
	};

	struct JoystickInfo
	{
		Unigine::InputJoystickPtr joystick;
		Unigine::String last_pressed_button;

		Unigine::VectorStack<FFBEffectData, Unigine::Input::NUM_JOYSTICK_FORCE_FEEDBACKS> ffb_effect_data;
		DurationEffectData ramp_effect_data;
	};

	void create_ffb_ui(int info_column, Unigine::InputJoystickPtr &joystick);

	Unigine::WidgetPtr create_ffb_effect_ui(const Unigine::String &name, JoystickInfo &info,
		Unigine::Input::JOYSTICK_FORCE_FEEDBACK_EFFECT effect_type, bool need_magnitude,
		bool need_duration, bool need_frequency,  const Unigine::WidgetPtr &parent);

	static bool is_ffb_supported(const Unigine::InputJoystickPtr &joystick);


	Unigine::Vector<JoystickInfo> joysticks_info;

	SimpleInformationBox *info = nullptr;
};
