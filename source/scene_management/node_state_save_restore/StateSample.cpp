// Demonstrates Node::saveState and Node::restoreState for serializing node state
// to a Blob. State can be saved and restored at runtime via UI buttons. A ghost
// box is rendered at the saved position to visualize the difference.

#include <UnigineComponentSystem.h>
#include <UnigineVisualizer.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

using namespace Unigine;
using namespace Math;

// Demonstrates saving and restoring node state via Blob serialization.
class StateSample : public ComponentBase
{
public:
	COMPONENT_DEFINE(StateSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Target node whose state will be saved/restored
	PROP_PARAM(Node, node_param);

private:
	// Blob is created for state storage and initial state is captured.
	void init()
	{
		saved_state = Blob::create();
		save_state();

		visualizer_enabled = Visualizer::isEnabled();
		Visualizer::setEnabled(true);
		gui.init(this);
	}

	// Ghost box is rendered at saved position for visual comparison.
	void update()
	{
		// Outer wireframe shows interaction bounds
		Visualizer::renderBox(vec3(2.f), translate(Vec3(0, 3, 1.3)), vec4_white);
		// Semi-transparent box shows saved position
		Visualizer::renderSolidBox(vec3(.75f), last_transform, vec4(vec3(1), .25f));
	}

	// Visualizer state is restored and UI is cleaned up.
	void shutdown()
	{
		gui.shutdown();
		Visualizer::setEnabled(visualizer_enabled);
	}

	// Node state is serialized to Blob and transform is cached for ghost rendering.
	void save_state()
	{
		// Clear previous state and serialize current node state to Blob
		saved_state->clear();
		node_param->saveState(saved_state);
		// Cache transform for ghost visualization
		last_transform = node_param->getWorldTransform();
	}

	// Node state is deserialized from Blob, restoring position and properties.
	void restore_state()
	{
		if (saved_state && saved_state->getSize())
		{
			// Reset stream position before reading
			saved_state->seekSet(0);
			node_param->restoreState(saved_state);
			// Reset for potential future reads
			saved_state->seekSet(0);
		}
	}

	// ========================================================================================

	// Nested GUI class for save/restore button management.
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

	// Saved visualizer state for restoration on shutdown
	bool visualizer_enabled = false;
	// Cached transform of saved state for ghost visualization
	Mat4 last_transform;
	// Binary storage for serialized node state
	BlobPtr saved_state;
	SampleGui gui;
};

REGISTER_COMPONENT(StateSample);
