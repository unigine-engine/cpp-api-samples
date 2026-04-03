// Spawns many nodes in background thread using AsyncQueue::runAsync(). Nodes are
// created with random positions off the main thread, then activated via main thread callback.

#include "AsyncQueueStressSample.h"

#include <UnigineAsyncQueue.h>
#include <UnigineGame.h>
#include <UnigineProfiler.h>

REGISTER_COMPONENT(AsyncQueueStressSample);

using namespace Unigine;
using namespace Math;

// Profiler is enabled and UI with load button is created.
void AsyncQueueStressSample::init()
{
	// Enable profiler to observe how background loading affects performance
	Profiler::setEnabled(true);

	num_nodes_loaded = 0;

	// Sample UI with description and controls
	sample_description_window.createWindow(Gui::ALIGN_RIGHT);

	WidgetGroupBoxPtr parameters = sample_description_window.getParameterGroupBox();
	
	auto num_nodes_hbox = WidgetHBox::create(5);
	parameters->addChild(num_nodes_hbox, Gui::ALIGN_EXPAND);

	auto multithread_label = WidgetLabel::create("Num nodes");
	num_nodes_hbox->addChild(multithread_label);

	auto spinbox_hbox = WidgetHBox::create();
	auto editline = WidgetEditLine::create();
	editline->setValidator(Gui::VALIDATOR_INT);
	auto spinbox = WidgetSpinBox::create();
	editline->addAttach(spinbox);
	spinbox->setMinValue(1);
	spinbox->setMaxValue(10000);
	spinbox->setValue(100);
	spinbox_hbox->addChild(editline);
	spinbox_hbox->addChild(spinbox);
	num_nodes_hbox->addChild(spinbox_hbox, Gui::ALIGN_RIGHT);

	auto request_load_nodes_button = WidgetButton::create("Request Load Nodes Async");
	parameters->addChild(request_load_nodes_button, Gui::ALIGN_EXPAND);

	num_nodes_loaded_label = WidgetLabel::create();
	parameters->addChild(num_nodes_loaded_label, Gui::ALIGN_EXPAND);

	// On click, request background node loading via AsyncQueue::runAsync()
	request_load_nodes_button->getEventClicked().connect(*this, [this, spinbox]()
	{
		AsyncQueue::runAsync(AsyncQueue::ASYNC_THREAD_BACKGROUND, MakeCallback(this, &AsyncQueueStressSample::load_nodes, spinbox->getValue()));
	});
}

// Loaded node count is displayed; label turns red if count exceeds threshold.
void AsyncQueueStressSample::update()
{
	// Update UI with current number of loaded nodes, highlight if load is high
	num_nodes_loaded_label->setText("Num nodes loaded " + String::ltoa(num_nodes_loaded));
	if (num_nodes_loaded > 2000)
		num_nodes_loaded_label->setFontColor(vec4_red);
}

// Profiler is disabled and UI window is closed.
void AsyncQueueStressSample::shutdown()
{
	// Disable profiler and clean up sample UI
	Profiler::setEnabled(false);
	sample_description_window.shutdown();
}

// Nodes are loaded in background thread; activation is scheduled on main thread.
void AsyncQueueStressSample::load_nodes(int num)
{
	// This method is executed in a background thread.
	// It creates nodes and assigns random positions without blocking the main thread.
	// Since nodes are created off the main thread, we must later call updateEnabled() 
	// on the main thread to register them in the engine (add to the spatial tree).
	for (int i = 0; i < num; ++i)
	{
		NodePtr loaded_node = World::loadNode(node_to_spawn.get(), false);

		// Assign a random position in the world
		Vec3 position;
		position.x = Game::getRandomFloat(-100.0f, 100.0f);
		position.y = Game::getRandomFloat(-100.0f, 100.0f);
		position.z = Game::getRandomFloat(0.0f, 50.0f);
		loaded_node->setWorldPosition(position);

		num_nodes_loaded++;

		// Schedule activation on main thread to register node in spatial tree
		AsyncQueue::runAsync(AsyncQueue::ASYNC_THREAD_MAIN, MakeCallback([loaded_node]()
			{
				// Call updateEnabled which will recursively go through all the children and add them to the spatial tree
				loaded_node->updateEnabled();
			}));
	}
}
