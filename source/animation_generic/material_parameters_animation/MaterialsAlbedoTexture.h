#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineMaterial.h>
#include <UnigineTextures.h>

// Alternates between two albedo textures on a timed interval.
class MaterialsAlbedoTexture : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(MaterialsAlbedoTexture, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	// Path to the first texture file (shown initially)
	PROP_PARAM(File, firstTextureImage);
	// Path to the second texture file (swapped to after timer expires)
	PROP_PARAM(File, secondTextureImage);
	// How long each texture is displayed before switching
	PROP_PARAM(Float, time, 5.0f);

	// Which of the two textures is applied right now (read by MaterialsParametersAnimationSample)
	bool isFirstTextureApplied() const { return time_sign > 0.0f; }

private:
	void init();
	void update();

private:
	// Cached reference to the object's material
	Unigine::MaterialPtr material;
	// Tracks elapsed time for texture switching
	float current_time = 0.0f;
	// Direction of time accumulation (1 = forward, -1 = backward for ping-pong)
	float time_sign = 1.0f;

	// Runtime texture objects loaded from file paths
	Unigine::TexturePtr first_texture;
	Unigine::TexturePtr second_texture;
};
