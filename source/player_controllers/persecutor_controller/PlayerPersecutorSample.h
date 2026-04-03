// Sample UI wrapper for the third-person persecutor camera demo.
// Creates controls for fixed mode, collision, distance limits, angles, and anchor.

#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

// Sets up mouse grab and creates GUI for adjusting PlayerPersecutorComponent parameters.
class PlayerPersecutorSample final : public Unigine::ComponentBase {
	COMPONENT_DEFINE(PlayerPersecutorSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, rotator, "");								// Target node to follow
	PROP_PARAM(Node, persecutor, "", "", "", "filter=PlayerDummy");	// Camera with component

private:
	void init();
	void shutdown();

private:
	SampleDescriptionWindow description_window;		// GUI window for parameter controls
	Unigine::Input::MOUSE_HANDLE mouse_handle_at_init;	// Original mouse mode for restoration
	bool current_mouse_grab_state{false};			// Unused
	bool mouse_grab_state_at_init{false};			// Original grab state for restoration

	Unigine::WidgetSliderPtr min_distance_slider;
	Unigine::WidgetSliderPtr max_distance_slider;
	Unigine::WidgetSliderPtr min_theta_slider;
	Unigine::WidgetSliderPtr max_theta_slider;
};
