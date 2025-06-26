#pragma once

#include "Persecutor.h"

#include <UnigineComponentSystem.h>

class MotionMode : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(MotionMode, Unigine::ComponentBase);
	COMPONENT_INIT(init);

	PROP_PARAM(Node, targetNode)
	PROP_PARAM(Node, persecutorNode)

protected:
	PersecutorBase *persecutor = nullptr;

protected:
	void init();
};