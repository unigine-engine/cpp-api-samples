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
		// create tracks and playbacks
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

		// all of the animation tracks, playbacks and objects are managed by the animation system and have the "engine lifetime",
		// i.e. they exist from the point they're loaded / created to when the engine shutdowns,
		// and therefore are preserved between different worlds

		// you have to stop active playbacks when switching to another world, if you don't want them continue playing there
		playback->stop();

		// restore render fade color
		Render::setBackgroundColor(vec4(0.f, 0.f, 0.f, 0.f));
	}

	void create_animations()
	{
		// create new track
		AnimationTrackPtr track = AnimationTrack::create();

		// animate gravity
		auto gravity_modifier = AnimationModifierFloat::create("physics.gravity_z");
		gravity_modifier->addValue(0.0f, -9.8f);
		gravity_modifier->addValue(3.0f, 2.5f);
		gravity_modifier->addValue(4.0f, -1.0f);
		gravity_modifier->addValue(5.0f, -4.5f);
		gravity_modifier->addValue(6.0f, -9.8f);
		track->addSingletonModifier(gravity_modifier);

		// animate fade color
		auto color_modifier = AnimationModifierFloat::create("render.background_color_w");
		color_modifier->addValue(0.0f, 0.0f);
		color_modifier->addValue(3.0f, 1.0f);
		color_modifier->addValue(4.0f, 1.0f);
		color_modifier->addValue(5.0f, 0.5f);
		color_modifier->addValue(6.0f, 0.0f);
		track->addSingletonModifier(color_modifier);

		// create track playback
		playback = AnimationPlayback::create();
		playback->setTrack(track);
		playback->setLoop(true);

		// you can save the created tracks and playbacks to disk if you want to use them later ...
		Dir::mkdir(FileSystem::getAbsolutePath(joinPaths(getWorldRootPath(), "tracks")));
		Animations::saveTrack(track, joinPaths(getWorldRootPath(), "tracks", "singletons.utrack"));
		Animations::savePlayback(playback, joinPaths(getWorldRootPath(), "tracks", "singletons.uplay"));

		// ... and then load them manually if they weren't loaded automatically at the engine initialization stage
		Animations::RESULT result = Animations::loadPlayback(joinPaths(getWorldRootPath(), "tracks", "singletons.uplay"));
		if (result != Animations::RESULT_PLAYBACK_ERROR)
		{
			// you can access tracks and playbacks from the animation system by their index, guid, file guid or file path
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
