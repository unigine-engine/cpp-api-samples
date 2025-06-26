#pragma once

#include <UnigineComponentSystem.h>
#include <UniginePlayers.h>

class CameraShiftController : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CameraShiftController, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	UNIGINE_INLINE void setModeShiftModeEnabled(bool enabled) { shift_enabled = enabled; }

private:
	void init();
	void update();
	void shutdown();

	Unigine::PlayerSpectatorPtr player;
	Unigine::PlayerDummyPtr dummy_player;

	void post_render_callback();
	void pre_render_callback();

	bool shift_enabled = false;
	bool need_reset_player = false;
};
