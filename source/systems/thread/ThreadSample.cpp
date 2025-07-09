#include "ThreadSample.h"

#include <UnigineConsole.h>

#include <memory>

using namespace Unigine;
using namespace Unigine::Math;

ThreadSample::InfiniteThread::InfiniteThread() {}

ThreadSample::InfiniteThread::~InfiniteThread() {}

void ThreadSample::InfiniteThread::process()
{
	while (isRunning())
	{
		Log::message("Hello from infinite thread\n");

		sleep(1000);
	}
	Log::message("Infinite thread finished\n");
}

ThreadSample::CountedThread::CountedThread(uint32_t iterations)
	: iterations(iterations)
{}

ThreadSample::CountedThread::~CountedThread() {}

void ThreadSample::CountedThread::process()
{
	for (uint32_t i = 0; i < iterations; ++i)
	{
		Log::message("Hello from counted thread: %u\n", i);

		if (!isRunning())
			break;

		sleep(1000);
	}

	Log::message("Counted thread finished\n");
}

//-----------------Sample-----------------

REGISTER_COMPONENT(ThreadSample)

void ThreadSample::init()
{
	Console::setOnscreen(true);

	infinite_thread = std::make_unique<InfiniteThread>();
	infinite_thread->run();

	counted_thread = std::make_unique<CountedThread>(10);
	counted_thread->run();
}

void ThreadSample::update()
{
	if (!counted_thread->isRunning())
		counted_thread->stop();
}

void ThreadSample::shutdown()
{
	Console::setOnscreen(false);
	if (infinite_thread->isRunning())
		infinite_thread->stop();
	if (counted_thread->isRunning())
		counted_thread->stop();
}
