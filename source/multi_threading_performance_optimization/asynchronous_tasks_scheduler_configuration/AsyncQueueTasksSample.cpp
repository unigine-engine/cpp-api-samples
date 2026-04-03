// Provides UI to test different AsyncQueue task execution modes. Supports single
// async tasks on various thread types and multithreaded tasks in sync/async modes.

#include "AsyncQueueTasksSample.h"

#include <UnigineAsyncQueue.h>
#include <UnigineConsole.h>

REGISTER_COMPONENT(AsyncQueueTasksSample);

using namespace Unigine;

// UI controls for thread type selection and task execution are created.
void AsyncQueueTasksSample::init()
{
	Console::setOnscreen(true);

	// Sample UI with description and controls
	sample_description_window.createWindow(Gui::ALIGN_RIGHT);

	WidgetGroupBoxPtr parameters = sample_description_window.getParameterGroupBox();
	
	// UI block to select which thread type the task will run on
	auto async_thread_type_hbox = WidgetHBox::create(5);
	parameters->addChild(async_thread_type_hbox, Gui::ALIGN_EXPAND);

	auto async_thread_type_label = WidgetLabel::create("Task Thread Type");
	async_thread_type_hbox->addChild(async_thread_type_label);

	auto async_thread_type_combobox = WidgetComboBox::create();
	async_thread_type_combobox->addItem("BACKGORUND");
	async_thread_type_combobox->addItem("ASYNC");
	async_thread_type_combobox->addItem("ASYNC FRAME POOL");
	async_thread_type_combobox->addItem("GPU STREAM");
	async_thread_type_combobox->addItem("FILE STREAM");
	async_thread_type_combobox->addItem("MAIN");
	async_thread_type_combobox->addItem("NEW");
	async_thread_type_hbox->addChild(async_thread_type_combobox, Gui::ALIGN_EXPAND);

	// Button to run a single asynchronous task in the selected thread type
	auto run_async_button = WidgetButton::create("Run Async");
	run_async_button->getEventClicked().connect(*this, [instance = this, async_thread_type_combobox]()
		{
			// Run a task asynchronously in a specified thread
			// You can also specify priority of your task
			// CRITICAL (high), DEFAULT (medium) or BACKGROUND (low)
			AsyncQueue::runAsync(AsyncQueue::ASYNC_THREAD(async_thread_type_combobox->getCurrentItem()), MakeCallback(instance, &AsyncQueueTasksSample::async_task));
		});
	parameters->addChild(run_async_button, Gui::ALIGN_EXPAND);

	auto spacer = WidgetSpacer::create();
	parameters->addChild(spacer, Gui::ALIGN_EXPAND);

	// UI block to control multithreaded task execution
	auto multithread_hbox = WidgetHBox::create(5);
	parameters->addChild(multithread_hbox, Gui::ALIGN_EXPAND);

	auto multithread_label = WidgetLabel::create("Num threads");
	multithread_hbox->addChild(multithread_label);

	auto spinbox_hbox = WidgetHBox::create();
	auto multithread_editline = WidgetEditLine::create();
	multithread_editline->setEditable(false);
	auto multithread_spinbox = WidgetSpinBox::create();
	multithread_editline->addAttach(multithread_spinbox);
	multithread_spinbox->setMinValue(1);
	multithread_spinbox->setMaxValue(20);
	multithread_spinbox->setValue(1);
	spinbox_hbox->addChild(multithread_editline);
	spinbox_hbox->addChild(multithread_spinbox);
	multithread_hbox->addChild(spinbox_hbox, Gui::ALIGN_RIGHT);

	// Checkbox to decide if multithreaded tasks should complete within the current frame
	auto frame_checkbox = WidgetCheckBox::create("Wait for multithreaded task to complete in frame");
	parameters->addChild(frame_checkbox, Gui::ALIGN_LEFT);

	// Button to run multithreaded tasks asynchronously (does not block caller)
	auto run_async_multithread_button = WidgetButton::create("Run Async Multithread");
	run_async_multithread_button->getEventClicked().connect(*this, [instance = this, multithread_spinbox, frame_checkbox]()
		{
			// Run a task in a multithread mode. Current thread number and total amount of threads are passed to the callback
			// Does not block the thread from which it is called
			if(frame_checkbox->isChecked())
				AsyncQueue::runFrameAsyncMultiThread(MakeCallback(instance, &AsyncQueueTasksSample::multithread_task), multithread_spinbox->getValue());
			else
				AsyncQueue::runAsyncMultiThread(MakeCallback(instance, &AsyncQueueTasksSample::multithread_task), multithread_spinbox->getValue());
		});
	parameters->addChild(run_async_multithread_button, Gui::ALIGN_EXPAND);

	// Button to run multithreaded tasks synchronously (blocks caller until completion)
	auto run_sync_multithread_button = WidgetButton::create("Run Sync Multithread");
	run_sync_multithread_button->getEventClicked().connect(*this, [instance = this, multithread_spinbox, frame_checkbox]()
		{
			// Run a task in a multithread mode. Current thread number and total amount of threads are passed to the callback
			// Blocks the thread from which it is called (the calling thread will be unblocked after the task is completed in all threads)
			if(frame_checkbox->isChecked())
				AsyncQueue::runFrameSyncMultiThread(MakeCallback(instance, &AsyncQueueTasksSample::multithread_task), multithread_spinbox->getValue());
			else	
				AsyncQueue::runSyncMultiThread(MakeCallback(instance, &AsyncQueueTasksSample::multithread_task), multithread_spinbox->getValue());
		});
	parameters->addChild(run_sync_multithread_button, Gui::ALIGN_EXPAND);
}

// Console is hidden and UI window is closed.
void AsyncQueueTasksSample::shutdown()
{
	// Hide console and clean up sample UI
	Console::setOnscreen(false);
	sample_description_window.shutdown();
}

// Single task executed on a worker thread; logs thread ID.
void AsyncQueueTasksSample::async_task()
{
	// Simulate workload
	Thread::sleep(200);
	Log::message("This is async task, thread id: %d\n", Thread::getID());
}

// Parallel task executed on multiple threads; logs current thread index and total.
void AsyncQueueTasksSample::multithread_task(int current_thread, int total_thread)
{
	// Simulate workload
	Thread::sleep(200);
	Log::message("This is multithread task(current thread: %d, total number of threads: %d), thread id: %d\n", current_thread, total_thread, Thread::getID());
}
