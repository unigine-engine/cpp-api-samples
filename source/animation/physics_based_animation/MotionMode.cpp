#include "MotionMode.h"

#include <UnigineObjects.h>

using namespace Unigine;
using namespace Math;

void MotionMode::init()
{
	if (!targetNode.get())
		Log::error("MotionMode::init(): cannot get targetNode property\n");

	persecutor = ComponentSystem::get()->getComponent<PersecutorBase>(persecutorNode.get());
	if (!persecutor)
		Log::error("MotionMode::init(): cannot get Persecutor component from persecutorNode\n");
}
