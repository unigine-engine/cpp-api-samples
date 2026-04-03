// Real-time procedural texture generation. Creates animated XOR-pattern
// textures by directly writing pixel data to Image objects each frame.
// The images are applied to object albedo slots dynamically.

#include "TexturesSample.h"

#include <UnigineGame.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(TexturesSample)

// Mesh objects are cached; base colors are set for each animated texture.
void TexturesSample::init()
{
	UNIGINE_ASSERT(meshes_param.size() == 4);

	// Cache mesh object references for efficient per-frame access
	for (int i = 0; i < 4; ++i)
	{
		ObjectPtr object = checked_ptr_cast<Object>(meshes_param[i].get());
		if (object)
		{
			meshes.push_back(object);
		}
		else
		{
			Log::warning("TexturesSample::init(): object is null");
		}
	}

	// Define distinct base colors for each texture (yellow, cyan, magenta, white)
	colors.push_back(ivec4(255, 255, 0, 255));
	colors.push_back(ivec4(0, 255, 255, 255));
	colors.push_back(ivec4(255, 0, 255, 255));
	colors.push_back(ivec4(255, 255, 255, 255));

	// Pre-allocate image array (actual images created lazily in update)
	images.resize(4);
}

// XOR pattern is regenerated each frame with time-based animation offset.
void TexturesSample::update()
{
	float time = Game::getTime();

	for (int i = 0; i < 4; ++i)
	{
		// Calculate animated offset using sine wave; each texture animates at slightly different speed
		int offset = static_cast<int>(Math::sin(time * (0.9f + i * 0.1f)) * 127.0f + 127.0f);

		// Lazy initialization of image on first frame
		ImagePtr &image = images[i];
		if (!image)
		{
			image = Image::create();
			image->create2D(128, 128, Image::FORMAT_RGBA8);
		}

		// Generate XOR pattern: classic demoscene effect using bitwise XOR of coordinates
		ivec4 color = colors[i];
		for (int y = 0; y < image->getHeight(); y++)
		{
			for (int x = 0; x < image->getWidth(); x++)
			{
				// XOR creates diamond-like pattern; offset and modulo add animation variation
				ivec4 c = color * ((x ^ y) * offset % 63) >> 6;
				image->set2D(x, y, Image::Pixel(c.x, c.y, c.z, c.w));
			}
		}

		// Apply updated image to mesh's albedo texture slot
		ObjectPtr &mesh = meshes[i];
		MaterialPtr material = mesh->getMaterialInherit(0);
		int num = material->findTexture("albedo");
		if (num != -1)
		{
			// Upload CPU image data to GPU texture
			material->setTextureImage(num, image);
		}
	}
}
