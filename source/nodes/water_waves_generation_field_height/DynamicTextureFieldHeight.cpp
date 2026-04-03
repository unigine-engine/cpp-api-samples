// Generates a dynamic heightfield texture using procedural sine waves. FieldHeight
// nodes modify terrain or water surfaces within their bounding volume. This component
// creates animated wave patterns by updating the R8 texture every frame.

#include "DynamicTextureFieldHeight.h"


REGISTER_COMPONENT(DynamicTextureFieldHeight);

using namespace Unigine;
using namespace Math;

void DynamicTextureFieldHeight::setImageSize(int size)
{
	size_map = size;
	// Format 0 = IMAGE_FORMAT_R8 (single channel, 8-bit)
	// Recreating the image discards old data and allocates new buffer
	image->create2D(size_map, size_map, 0);
}


void DynamicTextureFieldHeight::setFrequency(float f)
{
	frequency = f;
}

void DynamicTextureFieldHeight::setSpeed(float s)
{
	speed = s;
}

void DynamicTextureFieldHeight::setAmplitude(float power)
{
	// Convert normalized 0-1 range to R8 pixel value (0-255)
	// 127 is half of 255, so power=1.0 gives maximum height offset
	amplitude = int(127.0f * power);
}

void DynamicTextureFieldHeight::setWidgetSprite(WidgetSpritePtr spr)
{
	sprite = spr;
}

void DynamicTextureFieldHeight::init()
{
	// checked_ptr_cast returns null if node is not a FieldHeight
	instance = checked_ptr_cast<FieldHeight>(node);
	if (instance)
	{
		image = Image::create();
		// Create single-channel 8-bit image for heightfield data
		// Value 128 = neutral height, <128 = lower, >128 = higher
		image->create2D(size_map, size_map, 0);

		// Create GPU texture and bind it to the FieldHeight node
		texture = Texture::create();
		texture->setImage(image);
		instance->setTexture(texture);
	}
	else
	{
		// Disable component if attached to wrong node type
		node->setEnabled(false);
		Log::error("Component can only be added to the Field Height");
	}
}

void DynamicTextureFieldHeight::update()
{
	// Game::getTime() returns total elapsed time in seconds
	float time = Game::getTime() * speed;

	// Direct pixel access for performance (no per-pixel function calls)
	unsigned char* pixels_ = image->getPixels();
	for (int j = 0; j < size_map; j++)
	{
		// sinFast returns [-1, 1], adding 1.0 shifts to [0, 2]
		// Result is clamped to 0-255 by unsigned char assignment
		float value = amplitude * (sinFast(j * frequency / size_map + time) + 1.0f);
		// Horizontal stripes: same value across entire row
		for (int i = 0; i < size_map; i++)
		{
			pixels_[i + (size_map * j)] = value;
		}
	}

	// Upload modified CPU image to GPU texture
	texture->setImage(image);
	// Update UI preview if connected
	if (sprite)
		sprite->setImage(image);
}


