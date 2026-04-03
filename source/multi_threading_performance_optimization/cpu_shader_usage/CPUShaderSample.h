#pragma once
#include <UnigineComponentSystem.h>
#include <UnigineVector.h>

#include <atomic>
#include <memory>

// Demonstrates asynchronous cluster updates using CPUShader.
// Uses double-buffered clusters: one displayed, one updated in background threads.
// At frame end, clusters swap so rendering always shows latest data without stalling.
class CPUShaderSample final : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(CPUShaderSample, ComponentBase)
	COMPONENT_DESCRIPTION("This component demonstrates asynchronous cluster updates using CPUShader.")

	COMPONENT_INIT(init)
	COMPONENT_SWAP(swap)

	// Mesh file used for creating ObjectMeshCluster instances (set in the Editor)
	PROP_PARAM(File, mesh)

private:

	// Represents a single cluster with both visible and async versions
	struct AsyncCluster
	{
		Unigine::Math::Vec3 offset;
		int seed{0};
		Unigine::Math::Random random;

		Unigine::Math::Scalar timer{100.f};
		Unigine::Vector<Unigine::Math::Mat4> transforms;

		// Two versions of each cluster are used:
		// - 'cluster' is displayed in the current frame
		// - 'cluster_async' is updated in the background for the next frame
		// At the end of the frame, the two clusters are swapped so the visible cluster
		// always shows the latest updated data without stalling rendering

		Unigine::ObjectMeshClusterPtr cluster;			
		Unigine::ObjectMeshClusterPtr cluster_async;	
		bool is_need_update{true};
		int frames{0};

		void update(const Unigine::ObjectMeshClusterPtr &cluster_);
		void swap();
		void update();
	};

	// CPUShader subclass that will run our code asynchronously.
	// To implement your asynchronous logic, override the process() method.
	// When runSync() or runAsync() is called, this method is automatically invoked on worker threads.
	struct UpdateClusterCPUShader final : Unigine::CPUShader
	{
		void process(int thread_num, int threads_count) override;
		void run();
		Unigine::Vector<AsyncCluster> clusters{};
		std::atomic<int> counter{};
	};

private:
	void init();         // Creates clusters and initializes CPUShader
	void swap() const;   // Launches async cluster update via CPUShader

private:
	std::unique_ptr<UpdateClusterCPUShader> shader;
};
