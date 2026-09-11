// Toggles a material's emission state on and off at regular intervals.
// Emission makes objects appear to glow by adding self-illumination that isn't
// affected by scene lighting. This creates a blinking/pulsing light effect
// useful for indicators, warning lights, or glowing objects.

#include "MaterialsEmission.h"

#include <UnigineGame.h>
#include <UnigineLog.h>
#include <UnigineObjects.h>

REGISTER_COMPONENT(MaterialsEmission);

using namespace Unigine;
using namespace Math;

// Gets material reference from the object this component is attached to.
void MaterialsEmission::init()
{
	// The component changes the material of its own node, so any other node type is a setup error
	ObjectPtr object = checked_ptr_cast<Object>(node);
	if (object.isValid() == false || object->getNumSurfaces() == 0)
	{
		Log::error("MaterialsEmission::init(): the component must be assigned to an object with a surface\n");
		return;
	}

	material = object->getMaterial(0);
}

// Toggles emission state when timer threshold is reached.
void MaterialsEmission::update()
{
	if (material.isValid() == false)
		return;

	current_time += Game::getIFps() * time_sign;
	if (current_time < 0.0f || current_time > time)
	{
		// Flip emission state: 0 becomes 1, 1 becomes 0
		material->setState("emission", material->getState("emission") == 0 ? 1 : 0);

		time_sign = -time_sign;
	}
}
