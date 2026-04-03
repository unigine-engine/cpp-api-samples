// Base class for motion algorithms. Stores references to target (laser pointer)
// and persecutor (cat character). Derived classes implement different easing
// behaviors: linear, ease-in, ease-out, spring, bounce.

#include "MotionMode.h"

#include <UnigineObjects.h>

using namespace Unigine;
using namespace Math;

// References to target and persecutor nodes are validated and stored.
void MotionMode::init()
{
	// Validate references to target (laser) and persecutor (cat) nodes.
	// These are set by the CatDemo component during runtime.
	if (!targetNode.get())
		Log::error("MotionMode::init(): cannot get targetNode property\n");

	// Get the PersecutorBase interface from the persecutor node.
	// This allows different persecutor implementations (skinned mesh, simple mesh, etc.)
	persecutor = ComponentSystem::get()->getComponent<PersecutorBase>(persecutorNode.get());
	if (!persecutor)
		Log::error("MotionMode::init(): cannot get Persecutor component from persecutorNode\n");
}
