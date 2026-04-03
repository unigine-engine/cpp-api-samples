// Updates many ObjectMeshCluster instances in parallel using CPUShader.
// Each cluster has a visible and async copy; they swap each frame for seamless updates.

#include "CPUShaderSample.h"

#include <UnigineGame.h>
#include <UnigineProfiler.h>

REGISTER_COMPONENT(CPUShaderSample)

using namespace Unigine;
using namespace Math;


constexpr int SIZE{30};
constexpr float MIN_SPACING{15.0f};  // Minimum gap between clusters
// Actual spacing
constexpr float SPACING = static_cast<float>(SIZE * 2 + 1) + MIN_SPACING;

void CPUShaderSample::AsyncCluster::update(const ObjectMeshClusterPtr &cluster_)
{
	UNIGINE_PROFILER_FUNCTION;

	// Generate transforms based on timer and random values
	random.setSeed(seed);
	int num = 0;

	for (int y = -SIZE; y <= SIZE; y++)
	{
		for (int x = -SIZE; x <= SIZE; x++)
		{
			const Scalar rand_timer = timer + random.getFloat(0, 100.0);
			auto pos = Vec3(static_cast<Scalar>(x), static_cast<Scalar>(y),
						   Math::cos(rand_timer) * 50.f)
				+ offset;
			transforms[num] = translate(pos) * rotateZ(rand_timer * 360.f)
				* scale(static_cast<Scalar>(5.0f));
			num++;
		}
	}

	// Rebuild cluster geometry from transforms
	cluster_->createMeshes(transforms);
}

void CPUShaderSample::AsyncCluster::swap()
{
	// Swap async and render clusters at the end of the frame
	UNIGINE_PROFILER_FUNCTION;

	timer += Game::getIFps();

	// Warm up a few frames to let streaming settle
	is_need_update = cluster->isVisibleCamera() || cluster->isVisibleShadow() || frames < 60;
	if (frames < 60)
	{
		frames++;
	}

	if (!is_need_update)
	{
		return;
	}

	// Swap active and async clusters
	cluster->swap(cluster_async);
	cluster->setEnabled(true);
	cluster_async->setEnabled(false);
}

void CPUShaderSample::AsyncCluster::update()
{
	// Update the cluster asynchronously if needed
	UNIGINE_PROFILER_FUNCTION;
	if (is_need_update)
	{
		update(cluster_async);
	}
}

void CPUShaderSample::UpdateClusterCPUShader::process(int thread_num, int threads_count)
{
	UNIGINE_PROFILER_FUNCTION;

	while (true)
	{
		const int num = counter.fetch_add(1);
		if (num >= clusters.size())
		{
			break;
		}
		clusters[num].update();
	}
}


// We call this method ourselves
//	runAsync - non-blocking. Work is scheduled in engine thread pool.
//		The frame will pause later at Engine::swap() until all tasks finish.
//	runSync - blocking. Execution waits here until finished.
void CPUShaderSample::UpdateClusterCPUShader::run()
{
	UNIGINE_PROFILER_FUNCTION;

	for (auto &c : clusters)
	{
		c.swap();
	}

	counter = 0;
	runAsync();
}

void CPUShaderSample::init()
{
	// Create CPUShader object
	shader = std::make_unique<UpdateClusterCPUShader>();
	Vector<AsyncCluster> &clusters = shader->clusters;
	clusters.resize(64);

	// Number of transforms per cluster
	const int num_objects = pow2(static_cast<int>(SIZE * 2 + 1));

	// Initialize clusters
	for (int i = 0; i < shader->clusters.size(); i++)
	{
		AsyncCluster &c = clusters[i];

		// Create main and async cluster copies
		const String mesh_path = mesh.get();
		c.cluster = ObjectMeshCluster::create(mesh_path);
		c.cluster->setMaterialParameterFloat4("albedo_color", Game::getRandomColor(), 0);
		c.cluster_async = static_ptr_cast<ObjectMeshCluster>(c.cluster->clone());

		// Enable only main cluster
		c.cluster->setEnabled(true);
		c.cluster_async->setEnabled(false);

		c.is_need_update = true;

		// Distribute clusters in grid pattern
		const float x = SPACING * getBit(i, 0) + SPACING * 2.0f * getBit(i, 2)
			+ SPACING * 4.0f * getBit(i, 4);
		const float y = SPACING * getBit(i, 1) + SPACING * 2.0f * getBit(i, 3)
			+ SPACING * 4.0f * getBit(i, 5);

		c.offset = Vec3(x, y, 0);
		c.seed = i;

		// Allocate transforms buffer
		c.transforms.resize(num_objects);

		// Build initial meshes
		c.update(c.cluster);

		clusters[i] = c;
	};
	Log::message("Num objects %d", num_objects * shader->clusters.size());
}

// Clusters are swapped and async update is scheduled for next frame.
void CPUShaderSample::swap() const
{
	// launch CPUShader work
	// note: runAsync schedules execution; if not finished by Engine::swap(), 
	//       swap() will block until completion
	shader->run();
}
