// Demonstrates implementing a custom external package for virtual file system integration.
// Custom packages extend the Package class and provide file enumeration, selection, and
// reading capabilities. Once registered via FileSystem::addExternPackage, package contents
// become accessible through virtual paths, enabling runtime asset loading from any source.

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
	// Random vec3 is generated within specified bounds.
	vec3 random_vec3(const vec3 &from, const vec3 &to)
	{
		return {
			Game::getRandomFloat(from.x, to.x),
			Game::getRandomFloat(from.y, to.y),
			Game::getRandomFloat(from.z, to.z)
		};
	}

	// Random vec3 is generated within symmetric bounds (-size/2 to +size/2).
	vec3 random_vec3(const vec3 &size)
	{
		return random_vec3(- size * .5f, size * .5f);
	}
}

// Custom Package implementation that generates virtual mesh files on-the-fly.
// All files share the same underlying box mesh data, but appear as separate
// numbered mesh files (0.mesh, 1.mesh, etc.) to the file system.
class ExternalPackage : public Package
{
public:
	// Package is initialized with specified file count and box mesh data is cached.
	ExternalPackage(int num_files)
	{
		this->num_files = num_files;
		file = File::create();

		// Create a simple box mesh and save it into a temporary file.
		// This file is used as a data source for all virtual meshes.
		MeshPtr mesh = Mesh::create();
		mesh->addBoxSurface("box", vec3 {1.f });

		StringStack<> path = joinPaths(getWorldRootPath(), ".temporary", "box.mesh");

		if (mesh->save(path))
		{
			file->open(path, "rb");
		}
	}

	virtual ~ExternalPackage() = default;

	// Total number of virtual files in this package is returned.
	virtual int getNumFiles() override
	{
		return num_files;
	}

	// Virtual file path is generated based on file index (e.g., "0.mesh", "1.mesh").
	virtual const char *getFilePath(int num) override
	{
		return String::format("%d.mesh", num);
	}

	// File is selected for reading; size is set if file exists in this package.
	virtual bool selectFile(const char *name, size_t &size) override
	{
		bool exists = findFile(name) > 0;

		if (exists)
			size = file->getSize();

		return exists;
	}

	// File data is read from the cached box mesh into the provided buffer.
	virtual bool readFile(unsigned char *data, size_t size) override
	{
		if (!file->isOpened())
			return false;

		// Reset to beginning and read entire mesh data
		file->seekSet(0);
		size_t written = file->read(data, size);

		return written == size;
	}

	// File existence is checked by matching name against generated file names.
	virtual int findFile(const char *name) const override
	{
		for (int i = 0; i < num_files; i += 1)
		{
			if (String::format("%d.mesh", i) == name)
				return 1;
		}

		return 0;
	}

	// File size is returned (same for all files since they share the same mesh data).
	virtual size_t getFileSize(int num) const override
	{
		return file->getSize();
	}

private:
	int num_files = 0; // Number of virtual files to expose
	FilePtr file;      // Handle to cached box mesh data
};

// Sample component that registers a custom package and spawns meshes from it.
// Demonstrates how external packages integrate with the virtual file system.
class ExternalPackageSample : public ComponentBase
{
public:
	COMPONENT_DEFINE(ExternalPackageSample, ComponentBase);
	COMPONENT_DESCRIPTION("This component demonstrates loading meshes from a custom " 
							"external package and spawning them randomly in the scene");

	COMPONENT_INIT(init);

private:
	// External package is registered and meshes are spawned at random positions.
	void init()
	{
		// Register the custom package with virtual file system
		FileSystem::addExternPackage("package", new ExternalPackage(num_files));

		// Load each virtual mesh file and place it randomly in the scene
		for (int i = 0; i < num_files; i += 1)
		{
			ObjectMeshStaticPtr mesh_static = ObjectMeshStatic::create(String::format("%d.mesh", i));

			Vec3 position = Vec3(::random_vec3({ 4.f, 4.f, 2.f })) + Vec3_up * 2.f;
			quat rotation = quat(Game::getRandomFloat(0.f, 360.f), Game::getRandomFloat(0.f, 360.f), Game::getRandomFloat(0.f, 360.f));

			mesh_static->setWorldTransform(Mat4 { rotation, position });
		}
	}

	constexpr static int num_files = 64; // Number of mesh instances to spawn
};

REGISTER_COMPONENT(ExternalPackageSample);
