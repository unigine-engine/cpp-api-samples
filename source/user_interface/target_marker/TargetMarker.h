// Off-screen target indicator using arrow and marker sprites.
// When target is visible on screen, a point marker highlights its position.
// When target is off-screen or behind camera, an arrow indicates direction.

#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineGame.h>

// Displays directional indicator pointing toward target node from camera view.
class TargetMarker : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TargetMarker, Unigine::ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Sprite image displayed as directional arrow when target is off-screen
	PROP_PARAM(File, arrowSprite);
	// Sprite image displayed as marker when target is visible on screen
	PROP_PARAM(File, pointSprite);

	// Node to track and indicate direction toward
	PROP_PARAM(Node, targetNode);

	// Normalized pivot offset for point marker positioning (0.5, 0.5 = center)
	PROP_PARAM(Vec2, pointPivot, Unigine::Math::vec2(0.5, 0.5));

protected:
	void init();
	void update();
	void shutdown();

private:
	// Calculates intersection point of direction ray with screen boundary rectangle
	void getIntersectionWithRect(int& x, int& y, int vec_x, int vec_y, int half_width, int half_height);

	// Arrow sprite shown when target is off-screen
	Unigine::WidgetSpritePtr arrow;
	// Point marker sprite shown when target is visible
	Unigine::WidgetSpritePtr point;

	// Cached reference to target node being tracked
	Unigine::NodePtr target;

	// Camera player used for screen projection calculations
	Unigine::PlayerPtr camera;

	// Cached dimensions for point marker sprite
	int point_width;
	int point_height;
	// Cached dimensions for arrow sprite
	int arrow_width;
	int arrow_half_width;
	int arrow_height;
	int arrow_half_height;

};