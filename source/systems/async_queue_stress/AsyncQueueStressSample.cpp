#include "AsyncQueueStressSample.h"

#include <UnigineAsyncQueue.h>
#include <UnigineGame.h>
#include <UnigineProfiler.h>

REGISTER_COMPONENT(AsyncQueueStressSample);

using namespace Unigine;
using namespace Math;

void AsyncQueueStressSample::init()
{
	Profiler::setEnabled(true);

	num_nodes_loaded = 0;

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

	request_load_nodes_button->getEventClicked().connect(*this, [this, spinbox]()
	{
		AsyncQueue::runAsync(AsyncQueue::ASYNC_THREAD_BACKGROUND, MakeCallback(this, &AsyncQueueStressSample::load_nodes, spinbox->getValue()));
	});
}

void AsyncQueueStressSample::update()
{
	num_nodes_loaded_label->setText("Num nodes loaded " + String::itoa(num_nodes_loaded));
	if (num_nodes_loaded > 2000)
		num_nodes_loaded_label->setFontColor(vec4_red);
}

void AsyncQueueStressSample::shutdown()
{
	Profiler::setEnabled(false);

	sample_description_window.shutdown();
}

void AsyncQueueStressSample::load_nodes(int num)
{
	for (int i = 0; i < num; ++i)
	{
		// here we are loading the node not in the main thread, so it will not be added to the spatial tree
		NodePtr loaded_node = World::loadNode(node_to_spawn.get(), false);
		Vec3 position;
		position.x = Game::getRandomFloat(-100.0f, 100.0f);
		position.y = Game::getRandomFloat(-100.0f, 100.0f);
		position.z = Game::getRandomFloat(0.0f, 50.0f);
		loaded_node->setWorldPosition(position);

		num_nodes_loaded++;

		AsyncQueue::runAsync(AsyncQueue::ASYNC_THREAD_MAIN, MakeCallback([loaded_node]()
			{
				// call updateEnabled which will recursively go through all the children and add them to the spatial tree
				loaded_node->updateEnabled();
			}));
	}
}
