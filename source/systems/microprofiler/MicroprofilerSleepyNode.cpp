#include "MicroprofilerSleepyNode.h"
#include <UnigineProfiler.h>

REGISTER_COMPONENT(MicroprofilerSleepyNode);

using namespace Unigine;

void MicroprofilerSleepyNode::init()
{
	UNIGINE_PROFILER_FUNCTION;
	sleepFor(1);
}

void MicroprofilerSleepyNode::updateAsyncThread()
{
	int id = Profiler::beginMicro(__FUNCTION__);
	sleepFor(2);
	Profiler::endMicro(id);
}

void MicroprofilerSleepyNode::updateSyncThread()
{
	UNIGINE_PROFILER_SCOPED(__FUNCTION__);
	sleepFor(0.2);
}

void MicroprofilerSleepyNode::update()
{
	ScopedProfiler profiler(__FUNCTION__);
	node->rotate(0.0f, 0.0f, 3.0f);
	sleepFor(0.2);
}

void MicroprofilerSleepyNode::postUpdate()
{
	UNIGINE_PROFILER_FUNCTION;
	sleepFor(0.05);
}

void MicroprofilerSleepyNode::updatePhysics()
{
	UNIGINE_PROFILER_FUNCTION;
	sleepFor(0.02);
}

void MicroprofilerSleepyNode::swap()
{
	UNIGINE_PROFILER_FUNCTION;
	sleepFor(0.01);
}

void MicroprofilerSleepyNode::shutdown()
{
	UNIGINE_PROFILER_FUNCTION;
	sleepFor(1);
}

void MicroprofilerSleepyNode::sleepFor(double ms)
{
	auto ct = Time::getMilliseconds();
	while (true)
	{
		auto t = Time::getMilliseconds();
		if (t - ct >= ms)
			return;
	}
}
