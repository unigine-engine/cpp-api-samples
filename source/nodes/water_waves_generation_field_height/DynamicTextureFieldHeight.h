// Generates a dynamic heightfield texture using procedural sine waves. FieldHeight
// nodes modify terrain or water surfaces within their bounding volume. This component
// creates animated wave patterns by updating the R8 texture every frame.

#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineFields.h>
#include <UnigineGame.h>


class DynamicTextureFieldHeight : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(DynamicTextureFieldHeight, ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

public:
	// Resizes the heightmap texture (must be power of 2)
	void setImageSize(int size);
	// Higher frequency = more wave peaks across the texture
	void setFrequency(float frequency);
	// Animation speed multiplier for wave motion
	void setSpeed(float speed);
	// Wave height as normalized 0-1 value
	void setAmplitude(float power);
	// Optional UI sprite to display the heightmap visually
	void setWidgetSprite(Unigine::WidgetSpritePtr sprite);

private:
	void init();
	void update();

	// Texture resolution (width and height are equal)
	int size_map{ 1024 };
	float speed{ 1.0f };
	float frequency{ 1.0f };
	// R8 format uses 0-255 range; 127 = half intensity
	int amplitude{ 127 };

	// CPU-side image data for pixel manipulation
	Unigine::ImagePtr image;
	// UI preview of the heightmap (optional)
	Unigine::WidgetSpritePtr sprite;
	// The FieldHeight node this component modifies
	Unigine::FieldHeightPtr instance;
	// GPU texture uploaded each frame with new data
	Unigine::TexturePtr texture;
	Unigine::ObjectGuiPtr gui;
};