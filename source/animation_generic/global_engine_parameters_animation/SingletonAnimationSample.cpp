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

// This component demonstrates singleton animation modifiers for global engine parameters.
// Unlike object modifiers that target specific nodes/materials, singleton modifiers
// animate global systems like Physics (gravity) and Render (background color).
// Important: Animation objects have "engine lifetime" - they persist across world changes.
// Active playbacks must be stopped when switching worlds to prevent unwanted effects.
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
		// Create tracks and playbacks
		create_animations();

		playback->play();

		gui.init();
	}

	void update()
	{
		gui.update();
	}

	void shutdown()
	{
		gui.shutdown();

		// Animation tracks, playbacks and objects have "engine lifetime" - they persist
		// from creation until engine shutdown and are preserved between different worlds.
		// Active playbacks must be stopped when switching worlds to prevent continued playback.
		playback->stop();

		// Render fade color is restored to transparent black
		Render::setBackgroundColor(vec4(0.f, 0.f, 0.f, 0.f));
	}

	// Create animation track with singleton modifiers for global engine parameters
	void create_animations()
	{
		// Create new track
		AnimationTrackPtr track = AnimationTrack::create();

		// Modifier for Physics::gravity Z component (global engine parameter)
		// addValue(time_sec, value) adds a keyframe: time in seconds and the value at that time
		auto gravity_modifier = AnimationModifierFloat::create("physics.gravity_z");
		gravity_modifier->addValue(0.0f, -9.8f);	// Normal gravity
		gravity_modifier->addValue(3.0f, 2.5f);		// Reverse (objects float up)
		gravity_modifier->addValue(4.0f, -1.0f);
		gravity_modifier->addValue(5.0f, -4.5f);
		gravity_modifier->addValue(6.0f, -9.8f);	// Normal gravity
		// addSingletonModifier() targets global engine state, no object binding needed
		track->addSingletonModifier(gravity_modifier);

		// Modifier for Render::backgroundColor alpha component (global engine parameter)
		// Alpha approaching 1 creates a fade-to-white effect
		auto color_modifier = AnimationModifierFloat::create("render.background_color_w");
		color_modifier->addValue(0.0f, 0.0f);
		color_modifier->addValue(3.0f, 1.0f);	// Full white fade
		color_modifier->addValue(4.0f, 1.0f);
		color_modifier->addValue(5.0f, 0.5f);
		color_modifier->addValue(6.0f, 0.0f);
		track->addSingletonModifier(color_modifier);

		playback = AnimationPlayback::create();
		playback->setTrack(track);
		playback->setLoop(true);

		// Tracks and playbacks can be serialized to disk for reuse
		Dir::mkdir(FileSystem::getAbsolutePath(joinPaths(getWorldRootPath(), "tracks")));
		Animations::saveTrack(track, joinPaths(getWorldRootPath(), "tracks", "singletons.utrack"));
		Animations::savePlayback(playback, joinPaths(getWorldRootPath(), "tracks", "singletons.uplay"));

		// Load saved playback back from disk (demonstrates serialization roundtrip)
		Animations::RESULT result = Animations::loadPlayback(joinPaths(getWorldRootPath(), "tracks", "singletons.uplay"));
		if (result != Animations::RESULT_PLAYBACK_ERROR)
		{
			// Access loaded animations by path, GUID, or index
			playback = Animations::getPlaybackByPath(joinPaths(getWorldRootPath(), "tracks", "singletons.uplay"));
		}
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

	AnimationPlaybackPtr playback;
	SampleGui gui;
};

REGISTER_COMPONENT(SingletonAnimationSample);
