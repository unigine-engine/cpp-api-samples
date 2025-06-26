#include <UnigineComponentSystem.h>
#include <UnigineVisualizer.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

using namespace Unigine;
using namespace Math;

class StateSample : public ComponentBase
{
public:
	COMPONENT_DEFINE(StateSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, node_param);

private:
	void init()
	{
		saved_state = Blob::create();
		save_state();

		visualizer_enabled = Visualizer::isEnabled();
		Visualizer::setEnabled(true);
		gui.init(this);
	}

	void update()
	{
		Visualizer::renderBox(vec3(2.f), translate(Vec3(0, 3, 1.3)), vec4_white);
		Visualizer::renderSolidBox(vec3(.75f), last_transform, vec4(vec3(1), .25f));
	}

	void shutdown()
	{
		gui.shutdown();
		Visualizer::setEnabled(visualizer_enabled);
	}

	void save_state()
	{
		// you can save and restore any Node's state using two Stream-based methods: Node::saveState and Node::restoreState

		saved_state->clear();
		node_param->saveState(saved_state);
		last_transform = node_param->getWorldTransform();
	}

	void restore_state()
	{
		if (saved_state && saved_state->getSize())
		{
			saved_state->seekSet(0);
			node_param->restoreState(saved_state);
			saved_state->seekSet(0);
		}
	}

	// ========================================================================================

	struct SampleGui : public EventConnections
	{
		void init(StateSample *sample)
		{
			this->sample = sample;

			sample_description_window.createWindow();

			auto parameters = sample_description_window.getParameterGroupBox();
			auto vbox = WidgetVBox::create();
			parameters->addChild(vbox);

			{
				auto hbox = WidgetHBox::create();

				save_btn = WidgetButton::create("Save");
				save_btn->getEventClicked().connect(this, &SampleGui::on_save_state_btn_clicked);
				hbox->addChild(save_btn);

				restore_btn = WidgetButton::create("Restore");
				restore_btn->getEventClicked().connect(this, &SampleGui::on_restore_state_btn_clicked);
				hbox->addChild(restore_btn);

				vbox->addChild(hbox);
			}
		}

		void shutdown()
		{
			sample_description_window.shutdown();
		}

		void on_save_state_btn_clicked(const WidgetPtr &widget, int mouse)
		{
			sample->save_state();
		}

		void on_restore_state_btn_clicked(const WidgetPtr &widget, int mouse)
		{
			sample->restore_state();
		}

		WidgetButtonPtr save_btn;
		WidgetButtonPtr restore_btn;

		StateSample *sample = nullptr;
		SampleDescriptionWindow sample_description_window;
	};

	// ========================================================================================

	bool visualizer_enabled = false;
	Mat4 last_transform;
	BlobPtr saved_state;
	SampleGui gui;
};

REGISTER_COMPONENT(StateSample);
