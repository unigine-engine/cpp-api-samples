// Animates a material's metalness parameter between two values over time.
// Metalness controls how metallic a surface appears: 0 = dielectric (plastic, wood),
// 1 = full metal (steel, gold). This creates a visual transformation effect
// as the surface transitions between non-metallic and metallic appearance.

#include "MaterialsMetalness.h"

#include <UnigineGame.h>
#include <UnigineLog.h>
#include <UnigineObjects.h>

REGISTER_COMPONENT(MaterialsMetalness);

using namespace Unigine;
using namespace Math;

// Gets material reference from the attached object.
void MaterialsMetalness::init()
{
	// The component changes the material of its own node, so any other node type is a setup error
	ObjectPtr object = checked_ptr_cast<Object>(node);
	if (object.isValid() == false || object->getNumSurfaces() == 0)
	{
		Log::error("MaterialsMetalness::init(): the component must be assigned to an object with a surface\n");
		return;
	}

	material = object->getMaterial(0);
}

// Interpolates metalness each frame based on animation progress.
void MaterialsMetalness::update()
{
	// A zero duration would turn the transition factor below into a division by zero
	if (material.isValid() == false || time <= 0.0f)
		return;

	// Advance animation time, reverse direction at boundaries
	current_time += Game::getIFps() * time_sign;
	if (current_time < 0.0f || current_time > time)
		time_sign = -time_sign;

	// Calculate interpolation factor and set metalness parameter
	material->setParameterFloat("metalness", lerp(beginMetalness.get(), endMetalness.get(), saturate(current_time / time)));
}
