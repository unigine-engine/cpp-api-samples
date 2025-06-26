#include "DynamicTextureFieldHeight.h"


REGISTER_COMPONENT(DynamicTextureFieldHeight);

using namespace Unigine;
using namespace Math;

void DynamicTextureFieldHeight::setImageSize(int size)
{
	size_map = size;
	// create R8 image
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
	// 127 = 255 / 2      255 max red channel value for R8      power = [0 , 1]
	amplitude = int(127.0f * power);
}

void DynamicTextureFieldHeight::setWidgetSprite(WidgetSpritePtr spr)
{
	sprite = spr;
}

void DynamicTextureFieldHeight::init()
{
	instance = checked_ptr_cast<FieldHeight>(node);
	if (instance)
	{
		image = Image::create();
		// create R8 image
		image->create2D(size_map, size_map, 0);

		texture = Texture::create();
		texture->setImage(image);
		instance->setTexture(texture);
	}
	else
	{
		node->setEnabled(false);
		Log::error("Component can only be added to the Field Height");
	}
}

void DynamicTextureFieldHeight::update()
{
	unsigned char* ptr_pixels = image->getPixels();

	double pi_tile = 2.0f * Math::Consts::PI;
	float time = Game::getTime() * speed;

	unsigned char* pixels_ = image->getPixels();
	for (int j = 0; j < size_map; j++)
	{
		float value = amplitude * (sinFast(j * frequency / size_map + time) + 1.0f);
		for (int i = 0; i < size_map; i++)
		{
			pixels_[i + (size_map * j)] = value;
		}
	}

	texture->setImage(image);
	if (sprite)
		sprite->setImage(image);
}


