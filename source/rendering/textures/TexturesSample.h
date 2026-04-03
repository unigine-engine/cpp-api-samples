// Demonstrates dynamic texture manipulation using the Image API.
// Creates animated color patterns by directly modifying pixel data
// and uploading the result to GPU textures each frame.

#pragma once

#include <UnigineComponentSystem.h>

class TexturesSample final : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(TexturesSample, ComponentBase)

	// Mesh objects that display the animated textures
	PROP_ARRAY(Node, meshes_param, "Meshes")

	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)

private:
	void init();
	void update();

private:
	// Cached mesh objects for texture assignment
	Unigine::VectorStack<Unigine::ObjectPtr, 4> meshes;
	// Base colors for each texture's animation
	Unigine::VectorStack<Unigine::Math::ivec4, 4> colors;
	// CPU-side images for pixel manipulation
	Unigine::VectorStack<Unigine::ImagePtr, 4> images;
};
