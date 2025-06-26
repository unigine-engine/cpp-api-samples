#pragma once

#include <UnigineComponentSystem.h>

class TexturesSample final : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(TexturesSample, ComponentBase)

	PROP_ARRAY(Node, meshes_param, "Meshes")

	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)

private:
	void init();
	void update();

private:
	Unigine::VectorStack<Unigine::ObjectPtr, 4> meshes;
	Unigine::VectorStack<Unigine::Math::ivec4, 4> colors;
	Unigine::VectorStack<Unigine::ImagePtr, 4> images;
};
