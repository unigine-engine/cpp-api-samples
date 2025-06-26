#pragma once

#include <UnigineComponentSystem.h>

#include <atomic>
#include <memory>

class ThreadSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(ThreadSample, ComponentBase)

	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

private:

	// Here you can see how to create custom thread

	// to implement your own thread, you need to inherit from
	// Unigine::Thread class, then you need to override process method,
	// this method will be called after thread started its job
	// you can start your thread via run method

	// below, you can see a couple examples how you can create your own threads and
	// run it

	class InfiniteThread final : public Unigine::Thread
	{
	public:
		InfiniteThread();
		~InfiniteThread();

	private:
		void process() override;
	};

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
	void init();
	void update();
	void shutdown();

private:
	std::unique_ptr<CountedThread> counted_thread;
	std::unique_ptr<InfiniteThread> infinite_thread;
};
