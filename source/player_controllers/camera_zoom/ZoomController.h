// Implements camera zoom by adjusting FOV, render distance scale, and mouse
// sensitivity in sync. Higher zoom narrows FOV while extending LOD distances
// and reducing sensitivity for stable aiming at long range.

#pragma once
#include "UnigineComponentSystem.h"

// Coordinates FOV, LOD distance, and input sensitivity for smooth zooming.
class ZoomController : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ZoomController, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	void focus_on_target(Unigine::NodePtr target);	// Orients camera toward target node
	void udpate_zoom_factor(float zoom_factor);		// Applies zoom: FOV, distance scale, sensitivity
	void reset();									// Resets zoom to 1x

private:
	float default_FOV = 60.0f;				// Original FOV before zoom
	float default_distance_scale = 1.0f;	// Original LOD distance multiplier
	float default_sensivity = 1.0f;			// Original mouse sensitivity
	float default_player_turning = 90.0f;	// Original turning speed (for Spectator/Actor)

	Unigine::PlayerPtr player;

	void init();
	void shutdown();
	void update_turning(float zoom_factor);	// Adjusts player turning for Spectator/Actor types
};