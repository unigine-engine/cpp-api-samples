#include "ImagesSample.h"

#include <UnigineGame.h>

using namespace Unigine;
using namespace Math;

/////////////////////////////////////////////////////////
// Image manipulation functions
/////////////////////////////////////////////////////////

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

void ImagesSample::image_update()
{
	float ifps = Game::getIFps();

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

	// here we get a raw representation of pixels
	// below you can find how we can change its values
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

				if (field > 1.f)
				{
					field = 1.f;
				}
				// here we find color of pixel
				unsigned char density = static_cast<unsigned char>(field * 255.f);

				// "d" is a pointer to all pixels of image
				// each element in array, at which "d" is pointing at
				// is a value of color channel in pixel
				// here is a representation of how it looks like
				// chars   d
				// ↓       ↓
				// RGBARGBARGBARGBARGBA
				*d++ = density; // R
				*d++ = density; // G
				*d++ = density; // B
				*d++ = density; // A

				// now d points to an R channel of the next pixel
			}
		}
	}
}


// ----------------Sample Logic-----------------
REGISTER_COMPONENT(ImagesSample)

void ImagesSample::init()
{
	image = Image::create();
	image->create3D(32, 32, 32, Image::FORMAT_RGBA8);

	image_init();

	ObjectVolumeBoxPtr object = ObjectVolumeBox::create(vec3(20.f));
	object->setMaterial(Materials::findManualMaterial("Unigine::volume_cloud_base"), "*");
	object->setMaterialState("samples", 2, 0);
	object->setTransform(translate(Vec3(0.f, 0.f, 1.f)));
	material = object->getMaterialInherit(0);
}

void ImagesSample::update()
{
	image_update();

	material->setTextureImage(material->findTexture("density_3d"), image);
}
