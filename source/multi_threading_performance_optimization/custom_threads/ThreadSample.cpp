// Shows custom Thread subclasses with infinite and counted iteration patterns.
// Threads log messages to demonstrate execution; proper shutdown handling is shown.

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


ThreadSample::CountedThread::CountedThread(uint32_t iterations) : iterations(iterations) {}
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

// Sample Logic

REGISTER_COMPONENT(ThreadSample)

// Both thread types are created and started.
void ThreadSample::init()
{
	// Create and start threads
	// run() calls overridden process() internally
	Console::setOnscreen(true);

	infinite_thread = std::make_unique<InfiniteThread>();
	infinite_thread->run();

	counted_thread = std::make_unique<CountedThread>(10);
	counted_thread->run();
}

// Counted thread is stopped once it finishes its iterations.
void ThreadSample::update()
{
	// Stop counted thread if it has finished its iterations
	if (!counted_thread->isRunning())
		counted_thread->stop();
}

// Both threads are stopped and console is hidden.
void ThreadSample::shutdown()
{
	// Ensure both threads are properly stopped during shutdown
	Console::setOnscreen(false);
	if (infinite_thread->isRunning())
		infinite_thread->stop();
	if (counted_thread->isRunning())
		counted_thread->stop();
}
