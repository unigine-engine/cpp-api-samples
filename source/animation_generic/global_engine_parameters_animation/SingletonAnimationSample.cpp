#include <UnigineGame.h>
#include <UnigineEngine.h>
#include <UnigineLogic.h>
#include <UnigineWorld.h>
#include <UnigineAnimation.h>
#include <UniginePrimitives.h>
#include <UnigineComponentSystem.h>

#include "../../menu_ui/SampleDescriptionWindow.h"
#include "../../utils/Utils.h"

using namespace Unigine;
using namespace Math;

// This component demonstrates singleton animation channels for global engine parameters.
// Unlike channels that target specific nodes/materials, a singleton channel needs no bind:
// it animates a global system like Physics (gravity) or Render (background color).
// Important: animation players have "engine lifetime" - they persist across world changes.
// Active players must be stopped when switching worlds to prevent unwanted effects.
class SingletonAnimationSample : public ComponentBase
{
public:
	COMPONENT_DEFINE(SingletonAnimationSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init()
	{
		// Create the sequence and its player
		create_animations();

		player->play();

		gui.init();
	}

	void update()
	{
		gui.update();
	}

	void shutdown()
	{
		gui.shutdown();

		// Animation sequences and players have "engine lifetime" - they persist
		// from creation until engine shutdown and are preserved between different worlds.
		// Active players must be stopped when switching worlds to prevent continued playback.
		player->stop();

		// Render fade color is restored to transparent black
		Render::setBackgroundColor(vec4(0.f, 0.f, 0.f, 0.f));
	}

	// Create animation sequence with singleton channels for global engine parameters
	void create_animations()
	{
		// Create new sequence
		sequence = AnimationSequence::create();

		// Channel for Physics::gravity Z component (global engine parameter)
		// addValue(time_sec, value) adds a keyframe: time in seconds and the value at that time
		AnimationChannelFloatPtr gravity_channel = AnimationChannelFloat::create("physics.gravity_z");
		gravity_channel->addValue(0.0f, -9.8f);	// Normal gravity
		gravity_channel->addValue(3.0f, 2.5f);	// Reverse (objects float up)
		gravity_channel->addValue(4.0f, -1.0f);
		gravity_channel->addValue(5.0f, -4.5f);
		gravity_channel->addValue(6.0f, -9.8f);	// Normal gravity
		// A channel left without a bind targets global engine state
		sequence->addChannel(gravity_channel);

		// Channel for Render::backgroundColor alpha component (global engine parameter)
		// Alpha approaching 1 creates a fade-to-white effect
		AnimationChannelFloatPtr color_channel = AnimationChannelFloat::create("render.background_color_w");
		color_channel->addValue(0.0f, 0.0f);
		color_channel->addValue(3.0f, 1.0f);	// Full white fade
		color_channel->addValue(4.0f, 1.0f);
		color_channel->addValue(5.0f, 0.5f);
		color_channel->addValue(6.0f, 0.0f);
		sequence->addChannel(color_channel);

		// Sequences can be serialized to disk for reuse, the ".seq" extension is required
		Dir::mkdir(FileSystem::getAbsolutePath(joinPaths(getWorldRootPath(), "sequences")));
		StringStack<> sequence_path = joinPaths(getWorldRootPath(), "sequences", "singletons.seq");
		sequence->setPath(sequence_path);

		if (sequence->save() == false)
			Log::warning("SingletonAnimationSample: can't save the sequence to \"%s\"\n", sequence_path.get());

		// A saved sequence can be played right from its file (serialization roundtrip)
		player = AnimationSequencePlayer::create(sequence_path);

		// The file may be unavailable, then the in-memory sequence is played instead
		if (player->hasSequence() == false)
			player = AnimationSequencePlayer::create(sequence);

		player->setLoop(true);
	}

	// ========================================================================================

	struct SampleGui
	{
		void init()
		{
			sample_description_window.createWindow();

			auto w = sample_description_window.getWindow();
			auto state_gbox = WidgetGroupBox::create("State", 9, 3);
			w->addChild(state_gbox);

			auto gridbox = WidgetGridBox::create(2);
			state_gbox->addChild(gridbox, Gui::ALIGN_LEFT);

			auto add_parameter = [](const WidgetGridBoxPtr &gridbox, const char *name) {
				auto hbox = WidgetHBox::create();

				hbox->addChild(WidgetLabel::create(name));
				hbox->addChild(WidgetHBox::create(6));

				gridbox->addChild(hbox, Gui::ALIGN_LEFT);

				auto bg_color = WidgetEditLine::create();
				bg_color->setEditable(false);
				bg_color->setFontVOffset(-2);
				bg_color->setFontColor(vec4(vec3(.9f), 1.f));
				bg_color->setWidth(50);

				gridbox->addChild(bg_color, Gui::ALIGN_LEFT);
				return bg_color;
			};

			bg_color = add_parameter(gridbox, "render.background_color.a");
			gravity = add_parameter(gridbox, "physics.gravity.z");
		}

		void update()
		{
			bg_color->setText(String::format("%.2f", Render::getBackgroundColor().w));
			gravity->setText(String::format("%.1f", Physics::getGravity().z));
		}

		void shutdown() { sample_description_window.shutdown(); }

		SampleDescriptionWindow sample_description_window;
		WidgetEditLinePtr bg_color;
		WidgetEditLinePtr gravity;
	};

	// A player does not own a sequence built in code, so the sample keeps it alive itself
	AnimationSequencePtr sequence;
	AnimationSequencePlayerPtr player;
	SampleGui gui;
};

REGISTER_COMPONENT(SingletonAnimationSample);
