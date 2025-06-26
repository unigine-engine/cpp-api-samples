#pragma once

#include <UnigineComponentSystem.h>

class MicroprofilerSleepyNode
	: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(MicroprofilerSleepyNode, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE_ASYNC_THREAD(updateAsyncThread);
	COMPONENT_UPDATE_SYNC_THREAD(updateSyncThread);
	COMPONENT_UPDATE(update);
	COMPONENT_POST_UPDATE(postUpdate);
	COMPONENT_UPDATE_PHYSICS(updatePhysics);
	COMPONENT_SWAP(swap);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void updateAsyncThread();
	void updateSyncThread();
	void update();
	void postUpdate();
	void updatePhysics();
	void swap();
	void shutdown();

	static void sleepFor(double ms);
};
