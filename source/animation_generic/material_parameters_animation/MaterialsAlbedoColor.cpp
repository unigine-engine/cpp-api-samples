// Smoothly animates a material's albedo color between two values over time.
// Uses linear interpolation to create a gradual color transition that ping-pongs
// back and forth. Perfect for breathing effects, status indicators, or any
// visual feedback that requires smooth color changes.

#include "MaterialsAlbedoColor.h"

#include <UnigineGame.h>
#include <UnigineLog.h>
#include <UnigineObjects.h>

REGISTER_COMPONENT(MaterialsAlbedoColor);

using namespace Unigine;
using namespace Math;

// Gets material reference from the attached object.
void MaterialsAlbedoColor::init()
{
	// The component changes the material of its own node, so any other node type is a setup error
	ObjectPtr object = checked_ptr_cast<Object>(node);
	if (object.isValid() == false || object->getNumSurfaces() == 0)
	{
		Log::error("MaterialsAlbedoColor::init(): the component must be assigned to an object with a surface\n");
		return;
	}

	material = object->getMaterial(0);
}

// Interpolates color each frame based on animation progress.
void MaterialsAlbedoColor::update()
{
	// A zero duration would turn the transition factor below into a division by zero
	if (material.isValid() == false || time <= 0.0f)
		return;

	// Advance animation time, reverse direction at boundaries
	current_time += Game::getIFps() * time_sign;
	if (current_time < 0.0f || current_time > time)
		time_sign = -time_sign;

	// Calculate interpolation factor (0 to 1) and blend between colors
	material->setParameterFloat4("albedo_color", lerp(beginColor.get(), endColor.get(), saturate(current_time / time)));
}
