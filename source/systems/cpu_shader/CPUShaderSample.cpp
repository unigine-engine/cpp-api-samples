#include "CPUShaderSample.h"

#include <UnigineGame.h>
#include <UnigineProfiler.h>

REGISTER_COMPONENT(CPUShaderSample)

using namespace Unigine;
using namespace Math;


constexpr int SIZE{30};

void CPUShaderSample::AsyncCluster::update(const ObjectMeshClusterPtr &cluster_)
{
	UNIGINE_PROFILER_FUNCTION;

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

	cluster_->createMeshes(transforms);
}

void CPUShaderSample::AsyncCluster::swap()
{
	UNIGINE_PROFILER_FUNCTION;

	timer += Game::getIFps();

	// TODO fix workaround (streaming issue)
	is_need_update = cluster->isVisibleCamera() || cluster->isVisibleShadow() || frames < 60;
	if (frames < 60)
	{
		frames++;
	}


	if (!is_need_update)
	{
		return;
	}

	cluster->swap(cluster_async);

	cluster->setEnabled(true);
	cluster_async->setEnabled(false);
}

void CPUShaderSample::AsyncCluster::update()
{
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


// we call this method ourselves
// runAsync will wait in Engine::swap()
// runSync will wait here and stop execution
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
	shader = std::make_unique<UpdateClusterCPUShader>();
	Vector<AsyncCluster> &clusters = shader->clusters;
	clusters.resize(64);

	const int num_objects = pow2(static_cast<int>(SIZE * 2 + 1));

	for (int i = 0; i < shader->clusters.size(); i++)
	{
		AsyncCluster &c = clusters[i];

		const String mesh_path = mesh.get();
		c.cluster = ObjectMeshCluster::create(mesh_path);
		c.cluster->setMaterialParameterFloat4("albedo_color", Game::getRandomColor(), 0);
		c.cluster_async = static_ptr_cast<ObjectMeshCluster>(c.cluster->clone());

		c.cluster->setEnabled(true);
		c.cluster_async->setEnabled(false);

		c.is_need_update = true;

		const float x = SIZE * 3.0f * getBit(i, 0) + SIZE * 6.0f * getBit(i, 2)
			+ SIZE * 12.0f * getBit(i, 4);
		const float y = SIZE * 3.0f * getBit(i, 1) + SIZE * 6.0f * getBit(i, 3)
			+ SIZE * 12.0f * getBit(i, 5);

		c.offset = Vec3(x, y, 0);
		c.seed = i;

		c.transforms.resize(num_objects);

		c.update(c.cluster);

		clusters[i] = c;
	};
	Log::message("Num objects %d", num_objects * shader->clusters.size());
}

void CPUShaderSample::swap() const
{
	// function will stop its execution only in next swap
	shader->run();
}
