#include "AsyncQueueTasksSample.h"

#include <UnigineAsyncQueue.h>
#include <UnigineConsole.h>

REGISTER_COMPONENT(AsyncQueueTasksSample);

using namespace Unigine;

void AsyncQueueTasksSample::init()
{
	Console::setOnscreen(true);

	// create sample UI
	sample_description_window.createWindow(Gui::ALIGN_RIGHT);

	WidgetGroupBoxPtr parameters = sample_description_window.getParameterGroupBox();
	
	auto async_thread_type_hbox = WidgetHBox::create(5);
	parameters->addChild(async_thread_type_hbox, Gui::ALIGN_EXPAND);

	auto async_thread_type_label = WidgetLabel::create("Task Thread Type");
	async_thread_type_hbox->addChild(async_thread_type_label);

	auto async_thread_type_combobox = WidgetComboBox::create();
	async_thread_type_combobox->addItem("BACKGORUND");
	async_thread_type_combobox->addItem("ASYNC");
	async_thread_type_combobox->addItem("GPU STREAM");
	async_thread_type_combobox->addItem("FILE STREAM");
	async_thread_type_combobox->addItem("MAIN");
	async_thread_type_combobox->addItem("NEW");
	async_thread_type_hbox->addChild(async_thread_type_combobox, Gui::ALIGN_EXPAND);

	auto run_async_button = WidgetButton::create("Run Async");
	run_async_button->getEventClicked().connect(*this, [instance = this, async_thread_type_combobox]()
		{
			// run a task asynchronously in a specified thread
			AsyncQueue::runAsync(AsyncQueue::ASYNC_THREAD(async_thread_type_combobox->getCurrentItem()), MakeCallback(instance, &AsyncQueueTasksSample::async_task));
		});
	parameters->addChild(run_async_button, Gui::ALIGN_EXPAND);

	auto spacer = WidgetSpacer::create();
	parameters->addChild(spacer, Gui::ALIGN_EXPAND);

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

	auto run_async_multithread_button = WidgetButton::create("Run Async Multithread");
	run_async_multithread_button->getEventClicked().connect(*this, [instance = this, multithread_spinbox]()
		{
			// run a task in a multithread mode, current thread number and total amount of thread are passed to the callback
			// does not block the thread from which it is called
			AsyncQueue::runAsyncMultiThread(MakeCallback(instance, &AsyncQueueTasksSample::multithread_task), multithread_spinbox->getValue());
		});
	parameters->addChild(run_async_multithread_button, Gui::ALIGN_EXPAND);

	auto run_sync_multithread_button = WidgetButton::create("Run Sync Multithread");
	run_sync_multithread_button->getEventClicked().connect(*this, [instance = this, multithread_spinbox]()
		{
			// run a task in a multithread mode, current thread number and total amount of thread are passed to the callback
			// blocks the thread from which it was called (the calling thread will be unblocked after the task is completed in all threads)
			AsyncQueue::runSyncMultiThread(MakeCallback(instance, &AsyncQueueTasksSample::multithread_task), multithread_spinbox->getValue());
		});
	parameters->addChild(run_sync_multithread_button, Gui::ALIGN_EXPAND);
}

void AsyncQueueTasksSample::shutdown()
{
	Console::setOnscreen(false);
	sample_description_window.shutdown();
}

void AsyncQueueTasksSample::async_task()
{
	// simulate task work
	Thread::sleep(200);

	Log::message("This is async task, thread id: %d\n", Thread::getID());
}

void AsyncQueueTasksSample::multithread_task(int current_thread, int total_thread)
{
	// simulate task work
	Thread::sleep(200);

	Log::message("This is multithread task(current thread: %d, total number of threads: %d), thread id: %d\n", current_thread, total_thread, Thread::getID());
}
