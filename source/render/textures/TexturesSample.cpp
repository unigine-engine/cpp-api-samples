#include "TexturesSample.h"

#include <UnigineGame.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(TexturesSample)

void TexturesSample::init()
{
	UNIGINE_ASSERT(meshes_param.size() == 4);

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

	colors.push_back(ivec4(255, 255, 0, 255));
	colors.push_back(ivec4(0, 255, 255, 255));
	colors.push_back(ivec4(255, 0, 255, 255));
	colors.push_back(ivec4(255, 255, 255, 255));

	images.resize(4);
}

void TexturesSample::update()
{
	float time = Game::getTime();

	for (int i = 0; i < 4; ++i)
	{
		int offset = static_cast<int>(Math::sin(time * (0.9f + i * 0.1f)) * 127.0f + 127.0f);

		ImagePtr &image = images[i];
		if (!image)
		{
			image = Image::create();
			image->create2D(128, 128, Image::FORMAT_RGBA8);
		}

		// update image
		ivec4 color = colors[i];
		for (int y = 0; y < image->getHeight(); y++)
		{
			for (int x = 0; x < image->getWidth(); x++)
			{
				ivec4 c = color * ((x ^ y) * offset % 63) >> 6;
				image->set2D(x, y, Image::Pixel(c.x, c.y, c.z, c.w));
			}
		}

		ObjectPtr &mesh = meshes[i];
		MaterialPtr material = mesh->getMaterialInherit(0);
		int num = material->findTexture("albedo");
		if (num != -1)
		{
			material->setTextureImage(num, image);
		}
	}
}
