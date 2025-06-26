#pragma once
#include <UnigineComponentSystem.h>

class WheelAttachment : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(WheelAttachment, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	PROP_PARAM(Node, attached_to_wheel);

private:
	Unigine::NodePtr wheel_parent;
	Unigine::Math::Mat4 wheel_local_transform;
	Unigine::Math::Mat4 track_local_transform;
	
private:
	void init();
	void update();
};
