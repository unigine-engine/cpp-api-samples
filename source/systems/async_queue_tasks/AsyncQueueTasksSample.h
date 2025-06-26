#pragma once

#include <UnigineComponentSystem.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

class AsyncQueueTasksSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AsyncQueueTasksSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();

	void async_task();
	void multithread_task(int current_thread, int total_threads);

	SampleDescriptionWindow sample_description_window;
};
