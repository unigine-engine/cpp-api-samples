// Procedural 3D volume texture generation. Animates density fields that
// bounce within a cube, writing RGBA values directly to a 3D image each
// frame. The image is applied to a volume cloud material for visualization.

#include "ImagesSample.h"

#include <UnigineGame.h>

using namespace Unigine;
using namespace Math;

// Initializes random fields (positions, velocities, radii) that drive the 3D density
void ImagesSample::image_init()
{
	positions.resize(num_fields);
	velocities.resize(num_fields);
	radiuses.resize(num_fields);

	for (int i = 0; i < num_fields; ++i)
	{
		positions[i].x = randFloat(0.f, size);
		positions[i].y = randFloat(0.f, size);
		positions[i].z = randFloat(0.f, size);

		velocities[i].x = randFloat(-velocity, velocity);
		velocities[i].y = randFloat(-velocity, velocity);
		velocities[i].z = randFloat(-velocity, velocity);

		radiuses[i] = randFloat(radius / 2, radius);
	}
}

// Move fields, update 3D image, and write raw pixel data
void ImagesSample::image_update()
{
	float ifps = Game::getIFps();

	// Update field positions, bounce inside cube
	for (int i = 0; i < num_fields; ++i)
	{
		vec3 p = positions[i] + velocities[i] * ifps;
		if (p.x < 0.f || p.x > size)
		{
			velocities[i].x = -velocities[i].x;
		}
		if (p.y < 0.f || p.y > size)
		{
			velocities[i].y = -velocities[i].y;
		}
		if (p.z < 0.f || p.z > size)
		{
			velocities[i].z = -velocities[i].z;
		}

		positions[i] += velocities[i] * ifps;
	}

	int width = image->getWidth();
	int height = image->getHeight();
	int depth = image->getDepth();

	// Get raw pointer to all pixels of the 3D texture.
	// Data is stored as an array of bytes in RGBA order
	unsigned char *d = image->getPixels3D();

	vec3 position;
	float iwidth = size / width;
	float iheight = size / height;
	float idepth = size / depth;
	for (int z = 0; z < depth; ++z)
	{
		position.z = static_cast<float>(z) * idepth;
		for (int y = 0; y < height; ++y)
		{
			position.y = static_cast<float>(y) * iheight;
			for (int x = 0; x < width; x++)
			{
				position.x = static_cast<float>(x) * iwidth;
				float field = 0.f;
				for (int i = 0; i < num_fields; i++)
				{
					float distance = distance2(positions[i], position);
					if (distance < radiuses[i])
					{
						field += radiuses[i] - distance;
					}
				}

				if (field > 1.f) field = 1.f;

				// Convert to 8-bit value
				unsigned char density = static_cast<unsigned char>(field * 255.f);

				// "d" is a pointer to all pixels of image
				// Each element in array, at which "d" is pointing at
				// is a value of color channel in pixel
				// here is a representation of how it looks like
				// chars   d
				// ↓       ↓
				// RGBARGBARGBARGBARGBA
				*d++ = density; // R
				*d++ = density; // G
				*d++ = density; // B
				*d++ = density; // A

				// Now "d" points to an R channel of the next pixel
			}
		}
	}
}


// Sample Logic
REGISTER_COMPONENT(ImagesSample)

void ImagesSample::init()
{
	// Create a small 3D RGBA texture and seed it with initial data
	image = Image::create();
	image->create3D(32, 32, 32, Image::FORMAT_RGBA8);
	image_init();

	// Create a volume box and apply manual material to it
	// see the "volume_cloud_base" article in docs for more info
	ObjectVolumeBoxPtr object = ObjectVolumeBox::create(vec3(20.f));
	object->setMaterial(Materials::findManualMaterial("Unigine::volume_cloud_base"), "*");
	object->setMaterialState("samples", 2, 0);
	object->setTransform(translate(Vec3(0.f, 0.f, 1.f)));
	material = object->getMaterialInherit(0);
}

void ImagesSample::update()
{
	// Refresh the 3D texture each frame with new density
	// written to our image
	image_update();
	material->setTextureImage(material->findTexture("density_3d"), image);
}
