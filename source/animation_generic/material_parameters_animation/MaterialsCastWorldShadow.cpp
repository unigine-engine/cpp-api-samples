// Toggles whether a material casts shadows in the world at regular intervals.
// When CastWorldShadow is disabled, the object becomes invisible to shadow
// calculations, which can be useful for performance optimization or visual
// effects where an object shouldn't cast shadows.

#include "MaterialsCastWorldShadow.h"

#include <UnigineGame.h>
#include <UnigineLog.h>
#include <UnigineObjects.h>

REGISTER_COMPONENT(MaterialsCastWorldShadow);

using namespace Unigine;
using namespace Math;

// Gets material reference from the attached object.
void MaterialsCastWorldShadow::init()
{
	// The component changes the material of its own node, so any other node type is a setup error
	ObjectPtr object = checked_ptr_cast<Object>(node);
	if (object.isValid() == false || object->getNumSurfaces() == 0)
	{
		Log::error("MaterialsCastWorldShadow::init(): the component must be assigned to an object with a surface\n");
		return;
	}

	material = object->getMaterial(0);
}

// Toggles shadow casting when timer threshold is reached.
void MaterialsCastWorldShadow::update()
{
	if (material.isValid() == false)
		return;

	current_time += Game::getIFps() * time_sign;
	if (current_time < 0.0f || current_time > time)
	{
		// Flip shadow casting state on/off
		material->setCastWorldShadow(material->isCastWorldShadow() == false);

		time_sign = -time_sign;
	}
}
