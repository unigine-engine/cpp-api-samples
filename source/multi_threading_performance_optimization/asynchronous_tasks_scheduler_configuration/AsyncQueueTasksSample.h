#pragma once

#include <UnigineComponentSystem.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

// Demonstrates running tasks via AsyncQueue on different thread types.
// Shows single async tasks, multithreaded tasks, and sync vs async execution modes.
class AsyncQueueTasksSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AsyncQueueTasksSample, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component demonstrates running single and multithreaded tasks "
		"using AsyncQueue, with options for thread type and async or sync execution.")

	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();     // Creates UI with task execution controls
	void shutdown(); // Hides console and closes UI

	// Single asynchronous task
	void async_task();

	// Multithreaded task callback. Receives the current thread index and total number of threads
	void multithread_task(int current_thread, int total_threads);

	// Sample UI with description and controls
	SampleDescriptionWindow sample_description_window;
};
