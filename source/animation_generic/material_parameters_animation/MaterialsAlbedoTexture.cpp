// Demonstrates how to swap between two albedo textures on a material at runtime.
// The component loads two textures from file paths and alternates between them
// on a timer, creating a texture-switching animation effect. Useful for things
// like blinking displays, animated surfaces, or visual state changes.

#include "MaterialsAlbedoTexture.h"

#include <UnigineGame.h>
#include <UnigineLog.h>
#include <UnigineObjects.h>

REGISTER_COMPONENT(MaterialsAlbedoTexture);

using namespace Unigine;
using namespace Math;

// Loads both textures and applies the first one to the material's albedo slot.
void MaterialsAlbedoTexture::init()
{
	// The component changes the material of its own node, so any other node type is a setup error
	ObjectPtr object = checked_ptr_cast<Object>(node);
	if (object.isValid() == false || object->getNumSurfaces() == 0)
	{
		Log::error("MaterialsAlbedoTexture::init(): the component must be assigned to an object with a surface\n");
		return;
	}

	material = object->getMaterial(0);

	// Load texture files into GPU-ready texture objects.
	// Both images are loaded once here: loading on every swap would cost a frame in the middle of the sample
	first_texture = Texture::create();
	if (first_texture->load(firstTextureImage) == false)
		Log::error("MaterialsAlbedoTexture::init(): cannot load %s\n", firstTextureImage.get());

	second_texture = Texture::create();
	if (second_texture->load(secondTextureImage) == false)
		Log::error("MaterialsAlbedoTexture::init(): cannot load %s\n", secondTextureImage.get());

	// Start with the first texture visible
	material->setTexture("albedo", first_texture);
}

// Swaps the albedo texture when the timer reaches the threshold.
void MaterialsAlbedoTexture::update()
{
	if (material.isValid() == false)
		return;

	// Accumulate time in current direction
	current_time += Game::getIFps() * time_sign;
	if (current_time < 0.0f || current_time > time)
	{
		// Swap to the other texture based on direction
		material->setTexture("albedo", isFirstTextureApplied() ? second_texture : first_texture);

		// Reverse direction for ping-pong effect
		time_sign = -time_sign;
	}
}
