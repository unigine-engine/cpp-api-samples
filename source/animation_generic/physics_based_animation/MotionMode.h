#pragma once

#include "Persecutor.h"

#include <UnigineComponentSystem.h>

// Base class for motion interpolation strategies.
// Each derived class implements a different easing/physics approach:
// - LinearMotion: constant speed movement
// - EaseInMotion: gradual acceleration
// - EaseOutMotion: gradual deceleration
// - EaseOutBounceMotion: bounce effect at destination
// - SpringMotion family: spring physics simulation
class MotionMode : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(MotionMode, Unigine::ComponentBase);
	COMPONENT_INIT(init);

	PROP_PARAM(Node, targetNode)		// The laser pointer to chase
	PROP_PARAM(Node, persecutorNode)	// The cat that does the chasing

protected:
	PersecutorBase *persecutor = nullptr;

protected:
	void init();
};