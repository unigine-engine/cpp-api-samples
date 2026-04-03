#pragma once

#include <UnigineComponentSystem.h>
#include <UniginePlayers.h>

class CameraShiftController : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CameraShiftController, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component modifies the camera projection to enable two-point perspective, "
				"using a dummy player for rendering while keeping the original player for input and movement.")

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Enable or disable two-point perspective mode
	UNIGINE_INLINE void setModeShiftModeEnabled(bool enabled) { shift_enabled = enabled; }

private:
	void init();
	void update();
	void shutdown();

	// Controlled player
	Unigine::PlayerSpectatorPtr player;
	// Dummy player used to override projection matrix while keeping input on the real player
	Unigine::PlayerDummyPtr dummy_player;

	void post_render_callback();
	void pre_render_callback();

	// Rendering callbacks: swap players before and after rendering
	bool shift_enabled = false;
	bool need_reset_player = false;
};
