#pragma once

#include <UnigineComponentSystem.h>

#include <atomic>
#include <memory>

// Demonstrates custom thread implementation using Unigine::Thread base class.
// Shows two patterns: infinite loop thread and iteration-counted thread.
class ThreadSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(ThreadSample, ComponentBase)
	COMPONENT_DESCRIPTION("This component demonstrates threaded execution by running one infinite thread "
			"and one counted thread with messages logging.")

	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

private:
	// To implement your own thread, inherit from Unigine::Thread class
	// Override the process() method - it will be called after thread starts
	// Start your thread via the run() method

	// A thread that runs continuously until stopped
	class InfiniteThread final : public Unigine::Thread
	{
	public:
		InfiniteThread();
		~InfiniteThread();

	private:
		void process() override;
	};

	// A thread that runs for a specific number of iterations
	class CountedThread final : public Unigine::Thread
	{
	public:
		CountedThread(uint32_t iterations);
		~CountedThread();

	protected:
		void process() override;

	private:
		uint32_t iterations{0};
	};

private:
	void init();     // Creates and starts both threads
	void update();   // Monitors counted thread completion
	void shutdown(); // Stops both threads

private:
	// Managed instance of counted thread
	std::unique_ptr<CountedThread> counted_thread;

	// Managed instance of infinite thread
	std::unique_ptr<InfiniteThread> infinite_thread;
};
