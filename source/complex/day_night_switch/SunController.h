#pragma once
#include <UnigineComponentSystem.h>
#include <UnigineLights.h>
#include <UnigineGame.h>
#include <UnigineConsole.h>


class SunController : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(SunController, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component controls sun based on it's own game time and various adjustable parameters");

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
public:
	Unigine::Event<float>& getEventOnTimeChanged() { return time_changed_event; };

	void setTime(int t);
	/// <summary>
	/// Returns time in minutes
	/// </summary>
	/// <returns></returns>
	int getTime() { return current_time; };
	bool isContinuous() { return is_continuous.get(); };
	void setContinuousEnabled(bool value);
	void setTimescale(float timescale);
	float getTimescale() { return timescale; };
	void refreshSunPosition();

private:
	PROP_PARAM(Toggle, is_continuous, true, "", "Is sun moving continuously");
	PROP_PARAM(Float, timescale, 20000.0f, "", "Scale of countinuous time rotation");

	Unigine::EventInvoker<float> time_changed_event;
	Unigine::Math::quat sun_init_tilt = Unigine::Math::quat_identity;


	float current_time = 720 * 60;//in minutes
	const int max_time_sec = 60 * 60 * 24;//60 seconds in 60 minutes in 24 hours
	void init();
	void update();
};