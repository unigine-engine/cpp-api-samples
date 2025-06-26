#include <UnigineComponentSystem.h>
#include <UnigineFileSystem.h>
#include <UnigineObjects.h>
#include <UnigineWorlds.h>
#include <UnigineGame.h>

#include "../../utils/Utils.h"

using namespace Unigine;
using namespace Math;

namespace
{
	vec3 random_vec3(const vec3 &from, const vec3 &to)
	{
		return {
			Game::getRandomFloat(from.x, to.x),
			Game::getRandomFloat(from.y, to.y),
			Game::getRandomFloat(from.z, to.z)
		};
	}

	vec3 random_vec3(const vec3 &size)
	{
		return random_vec3(- size * .5f, size * .5f);
	}
}

class ExternalPackage : public Package
{
public:
	ExternalPackage(int num_files)
	{
		this->num_files = num_files;
		file = File::create();

		MeshPtr mesh = Mesh::create();
		mesh->addBoxSurface("box", vec3 {1.f });

		StringStack<> path = joinPaths(getWorldRootPath(), ".temporary", "box.mesh");

		if (mesh->save(path))
		{
			file->open(path, "rb");
		}
	}

	virtual ~ExternalPackage() = default;

	virtual int getNumFiles() override
	{
		return num_files;
	}

	virtual const char *getFilePath(int num) override
	{
		return String::format("%d.mesh", num);
	}

	virtual bool selectFile(const char *name, size_t &size) override
	{
		bool exists = findFile(name);

		if (exists)
			size = file->getSize();

		return exists;
	}

	virtual bool readFile(unsigned char *data, size_t size) override
	{
		if (!file->isOpened())
			return false;

		file->seekSet(0);
		size_t written = file->read(data, size);

		return written == size;
	}

	virtual int findFile(const char *name) const override
	{
		for (int i = 0; i < num_files; i += 1)
		{
			if (String::format("%d.mesh", i) == name)
				return 1;
		}

		return 0;
	}

	virtual size_t getFileSize(int num) const override
	{
		return file->getSize();
	}

private:
	int num_files = 0;
	FilePtr file;
};

class ExternalPackageSample : public ComponentBase
{
public:
	COMPONENT_DEFINE(ExternalPackageSample, ComponentBase);
	COMPONENT_INIT(init);

private:
	void init()
	{
		FileSystem::addExternPackage("package", new ExternalPackage(num_files));

		for (int i = 0; i < num_files; i += 1)
		{
			ObjectMeshStaticPtr mesh_static = ObjectMeshStatic::create(String::format("%d.mesh", i));

			Vec3 position = Vec3(::random_vec3({ 4.f, 4.f, 2.f })) + Vec3_up * 2.f;
			quat rotation = quat(Game::getRandomFloat(0.f, 360.f), Game::getRandomFloat(0.f, 360.f), Game::getRandomFloat(0.f, 360.f));

			mesh_static->setWorldTransform(Mat4 { rotation, position });
		}
	}
	// ========================================================================================

	constexpr static int num_files = 64;
};

REGISTER_COMPONENT(ExternalPackageSample);
