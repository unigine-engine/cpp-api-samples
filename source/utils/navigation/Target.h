#pragma once

#include <UnigineComponentSystem.h>

class PathfindingTarget : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(PathfindingTarget, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	
	PROP_PARAM(Float, radius, 0.5f);
	PROP_PARAM(Node, positions);
	
	void onReached();
	
private:
	void init();
	void take_new_position();
};
