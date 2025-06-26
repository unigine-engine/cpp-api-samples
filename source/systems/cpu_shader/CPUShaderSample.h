#pragma once
#include <UnigineComponentSystem.h>
#include <UnigineVector.h>

#include <atomic>
#include <memory>

class CPUShaderSample final : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(CPUShaderSample, ComponentBase)
	COMPONENT_INIT(init)
	COMPONENT_SWAP(swap)

	PROP_PARAM(File, mesh)

private:

	// helper class we will use to store all cluster information and use
	// to update clusters
	struct AsyncCluster
	{
		Unigine::Math::Vec3 offset;
		int seed{0};
		Unigine::Math::Random random;

		Unigine::Math::Scalar timer{100.f};
		Unigine::Vector<Unigine::Math::Mat4> transforms;

		// we use different clusters
		// cluster is used for showing on a frame
		// and cluster_async is used to calculate all cluster positions
		// on the next frame and at the end of frame we swap
		// our clusters
		Unigine::ObjectMeshClusterPtr cluster;
		Unigine::ObjectMeshClusterPtr cluster_async;
		bool is_need_update{true};
		int frames{0};

		void update(const Unigine::ObjectMeshClusterPtr &cluster_);
		void swap();
		void update();
	};

	// cpu shader, that will run our code asynchronously
	// to set up your async logic, you need to overwrite process() method
	// so when you call runSync() or runAsync(), this method will be called
	struct UpdateClusterCPUShader final : Unigine::CPUShader
	{
		void process(int thread_num, int threads_count) override;
		void run();
		Unigine::Vector<AsyncCluster> clusters{};
		std::atomic<int> counter{};
	};

private:
	void init();
	void swap() const;

private:
	std::unique_ptr<UpdateClusterCPUShader> shader;
};
