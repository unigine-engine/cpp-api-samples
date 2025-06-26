#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineGame.h>

class TargetMarker : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TargetMarker, Unigine::ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(File, arrowSprite);
	PROP_PARAM(File, pointSprite);

	PROP_PARAM(Node, targetNode);

	PROP_PARAM(Vec2, pointPivot, Unigine::Math::vec2(0.5, 0.5));

protected:
	void init();
	void update();
	void shutdown();

private:
	void getIntersectionWithRect(int& x, int& y, int vec_x, int vec_y, int half_width, int half_height);

	Unigine::WidgetSpritePtr arrow;
	Unigine::WidgetSpritePtr point;

	Unigine::NodePtr target;

	Unigine::PlayerPtr camera;

	int point_width;
	int point_height;
	int arrow_width;
	int arrow_half_width;
	int arrow_height;
	int arrow_half_height;

};