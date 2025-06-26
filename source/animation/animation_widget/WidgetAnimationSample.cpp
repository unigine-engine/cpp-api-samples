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

class WidgetAnimationSample : public ComponentBase
{
public:
	COMPONENT_DEFINE(WidgetAnimationSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init()
	{
		// create tracks and playback
		create_animations();

		// create widgets
		EngineWindowViewportPtr main_window = WindowManager::getMainWindow();
		widgets = WidgetVBox::create();

		WidgetPtr player_0_label = WidgetLabel::create("Player 0");
		player_0_label->setFontOutline(1);
		player_0_label->setFontSize(100);
		player_0_label->setPosition(250, 300);
		player_0_label->setFontColor(vec4_red);
		widgets->addChild(player_0_label, Gui::ALIGN_OVERLAP);

		WidgetPtr player_1_label = WidgetLabel::create("Player 1");
		player_1_label->setFontOutline(1);
		player_1_label->setFontSize(100);
		player_1_label->setPosition(975, 500);
		player_1_label->setFontColor(vec4_green);
		widgets->addChild(player_1_label, Gui::ALIGN_OVERLAP);

		WidgetPtr vs_label = WidgetLabel::create("vs");
		vs_label->setFontOutline(1);
		vs_label->setFontSize(100);
		vs_label->setPosition(750, 400);
		widgets->addChild(vs_label, Gui::ALIGN_OVERLAP);

		main_window->addChild(widgets, Gui::ALIGN_OVERLAP);

		// set up runtime binds in the runtime animation objects
		{
			AnimationTrackPtr track = playback->getTrack();

			AnimationObjectRuntimePtr anim_obj;
			AnimationBindRuntimePtr bind;

			anim_obj = checked_ptr_cast<AnimationObjectRuntime>(track->getObject(0));
			bind = anim_obj->getBind();
			bind->setWidget(player_0_label);
			anim_obj->setBind(bind);

			anim_obj = checked_ptr_cast<AnimationObjectRuntime>(track->getObject(1));
			bind = anim_obj->getBind();
			bind->setWidget(player_1_label);
			anim_obj->setBind(bind);

			anim_obj = checked_ptr_cast<AnimationObjectRuntime>(track->getObject(2));
			bind = anim_obj->getBind();
			bind->setWidget(vs_label);
			anim_obj->setBind(bind);
		}

		playback->play();
	}

	void shutdown()
	{
		widgets.deleteLater();

		playback->stop();
	}

	void create_animations()
	{
		// create new track
		AnimationTrackPtr track = AnimationTrack::create();

		// create runtime animation objects
		AnimationObjectRuntimePtr player_0_obj = AnimationObjectRuntime::create("widget_player_0");
		AnimationObjectRuntimePtr player_1_obj = AnimationObjectRuntime::create("widget_player_1");
		AnimationObjectRuntimePtr vs_obj = AnimationObjectRuntime::create("widget_vs");

		track->addObject(player_0_obj);
		track->addObject(player_1_obj);
		track->addObject(vs_obj);

		// create modifiers
		auto player_0_pos_modifier = AnimationModifierInt::create("widget.position_x");
		player_0_pos_modifier->addValue(0.0f, 0);
		player_0_pos_modifier->addValue(0.25f, 250);
		player_0_pos_modifier->addValue(2.75f, 250);
		player_0_pos_modifier->addValue(3.0f, 0);
		player_0_pos_modifier->addValue(5.0f, 0);
		track->addObjectModifier(player_0_obj, player_0_pos_modifier);

		auto player_1_pos_modifier = AnimationModifierInt::create("widget.position_x");
		player_1_pos_modifier->addValue(0.0f, 1225);
		player_1_pos_modifier->addValue(0.25f, 975);
		player_1_pos_modifier->addValue(2.75f, 975);
		player_1_pos_modifier->addValue(3.0f, 1225);
		player_1_pos_modifier->addValue(5.0f, 1225);
		track->addObjectModifier(player_1_obj, player_1_pos_modifier);

		auto player_color_modifier = AnimationModifierFloat::create("widget.font_color_w");
		player_color_modifier->addValue(0.0f, 0.001f);
		player_color_modifier->addValue(0.25f, 1.0f);
		player_color_modifier->addValue(2.75f, 1.0f);
		player_color_modifier->addValue(3.0f, 0.001f);
		player_color_modifier->addValue(5.0f, 0.001f);
		track->addObjectModifier(player_0_obj, player_color_modifier);
		track->addObjectModifier(player_1_obj, player_color_modifier);

		auto player_font_size_modifier = AnimationModifierInt::create("widget.font_size");
		player_font_size_modifier->addValue(0.0f, 80);
		player_font_size_modifier->addValue(0.25f, 100);
		player_font_size_modifier->addValue(2.75f, 100);
		player_font_size_modifier->addValue(3.0f, 80);
		player_font_size_modifier->addValue(5.0f, 80);
		track->addObjectModifier(player_0_obj, player_font_size_modifier);
		track->addObjectModifier(player_1_obj, player_font_size_modifier);

		auto vs_color_modifier = AnimationModifierFVec4::create("widget.font_color");
		vs_color_modifier->addValue(0.0f, vec4(0.0f, 1.0f, 1.0f, 0.001f));
		vs_color_modifier->addValue(0.25f, vec4(0.0f, 1.0f, 1.0f, 0.001f));
		vs_color_modifier->addValue(0.3f, vec4(0.0f, 1.0f, 1.0f, 1.0f));
		vs_color_modifier->addValue(1.5f, vec4(1.0f, 1.0f, 0.0f, 1.0f));
		vs_color_modifier->addValue(2.70f, vec4(0.0f, 1.0f, 1.0f, 1.0f));
		vs_color_modifier->addValue(2.75f, vec4(0.0f, 1.0f, 1.0f, 0.001f));
		vs_color_modifier->addValue(3.0f, vec4(0.0f, 1.0f, 1.0f, 0.001f));
		vs_color_modifier->addValue(5.0f, vec4(0.0f, 1.0f, 1.0f, 0.001f));
		track->addObjectModifier(vs_obj, vs_color_modifier);

		// create playback for track
		playback = AnimationPlayback::create();
		playback->setTrack(track);
		playback->setLoop(true);
	}

	// ========================================================================================

	AnimationPlaybackPtr playback;
	WidgetPtr widgets;
};

REGISTER_COMPONENT(WidgetAnimationSample);
