#pragma once

#include <UnigineThread.h>
#include <UnigineVector.h>
#include <UnigineString.h>
#include <atomic>

/// helpers for Async Task in separate thread
/// how to use:
/// just create instance of AsyncTask
/// auto task = new AsyncTask<int>([]()->int { // instead of int you can use any copied type
///    // this code will be invoked in separeted thread
///    return 1;
/// });
/// and than you can check task->isCompleated() in main thread, and get result by task->get();
///
/// for example
/// std::unique_ptr<AsyncTask<double>> task;
///
/// void update()
///{
///	if (Input::isKeyPressed(Input::KEY_Z))
///	{
///		// create task
///		task = std::make_unique<AsyncTask<double>>([]()->double {
///			Log::message("async task started\n");
///			double ret = 1;
///			for (int i = 0; i < 200000; i++) // any long-time task for few frames
///				ret += 1.0/i;
///			Log::message("async task ended\n");
///			return ret;
///		});
///		Log::message("create async task\n");
///	}
///	if (task && task->isCompleted()) // check in loop if compleated
///	{
///		Log::message("task compleated. value in main thread %f\n", task->get()); // take results
///		task.reset();
///	}
///}


class AsyncTaskBase;

class ThreadQueue: public Unigine::Thread
{
public:
	static ThreadQueue &get();

	void add(AsyncTaskBase *task);
	void remove(AsyncTaskBase *task);

protected:
	Unigine::Mutex mutex;
	Unigine::Vector<AsyncTaskBase *> queue;
	ThreadQueue() {}
	~ThreadQueue() {}
	virtual void process() override;
};

class AsyncTaskBase
{
	friend ThreadQueue;

public:
	AsyncTaskBase();
	virtual ~AsyncTaskBase();

	bool isCompleted() const { return completed; }

protected:
	std::atomic_bool completed{false};
	virtual void process() = 0;
};

template <typename T>
class AsyncTask final: public AsyncTaskBase
{
public:
	AsyncTask(std::function<T()> &&task)
		: AsyncTaskBase()
		, function(std::move(task))
	{
		UNIGINE_ASSERT(ThreadQueue::get().isRunning());
		ThreadQueue::get().add(this);
	};

	virtual ~AsyncTask() override
	{
		Unigine::ScopedLock lock(mutex);
	};

	T &get() { return stored_value; }
	const T &get() const { return stored_value; }
	operator T &() { return stored_value; }
	operator T const &() const { return stored_value; }

private:
	Unigine::Mutex mutex;
	T stored_value;
	std::function<T()> function;
	virtual void process() override
	{
		Unigine::ScopedLock lock(mutex);
		stored_value = function();
		completed = true;
	}
};
