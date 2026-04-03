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

// This component demonstrates widget animation using AnimationObjectRuntime.
// Unlike node/material animation objects, runtime objects can target any C++ object
// (widgets, custom classes) by binding directly to object pointers at runtime.
// Shows animating widget position, font size, and color properties.
class WidgetAnimationSample : public ComponentBase
{
public:
	COMPONENT_DEFINE(WidgetAnimationSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init()
	{
		// Create tracks and playback
		create_animations();

		// Create widgets
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

		// Runtime binding: connect animation objects to actual widget instances
		// This must be done at runtime since widgets are created dynamically
		{
			AnimationTrackPtr track = playback->getTrack();

			AnimationObjectRuntimePtr anim_obj;
			AnimationBindRuntimePtr bind;

			// Bind first animation object to Player 0 label
			anim_obj = checked_ptr_cast<AnimationObjectRuntime>(track->getObject(0));
			bind = anim_obj->getBind();
			bind->setWidget(player_0_label);	// Direct pointer binding
			anim_obj->setBind(bind);

			// Bind second animation object to Player 1 label
			anim_obj = checked_ptr_cast<AnimationObjectRuntime>(track->getObject(1));
			bind = anim_obj->getBind();
			bind->setWidget(player_1_label);
			anim_obj->setBind(bind);

			// Bind third animation object to "vs" label
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

	// Create animation track for "Player 0 vs Player 1" title animation
	void create_animations()
	{
		AnimationTrackPtr track = AnimationTrack::create();

		// AnimationObjectRuntime - generic runtime objects for widgets and custom targets
		// Unlike node/material objects, these don't use asset-based binding
		AnimationObjectRuntimePtr player_0_obj = AnimationObjectRuntime::create("widget_player_0");
		AnimationObjectRuntimePtr player_1_obj = AnimationObjectRuntime::create("widget_player_1");
		AnimationObjectRuntimePtr vs_obj = AnimationObjectRuntime::create("widget_vs");

		track->addObject(player_0_obj);
		track->addObject(player_1_obj);
		track->addObject(vs_obj);

		// Slide Player 0 label in from left
		auto player_0_pos_modifier = AnimationModifierInt::create("widget.position_x");
		player_0_pos_modifier->addValue(0.0f, 0);		// Off-screen left
		player_0_pos_modifier->addValue(0.25f, 250);	// Slide in
		player_0_pos_modifier->addValue(2.75f, 250);	// Hold
		player_0_pos_modifier->addValue(3.0f, 0);		// Slide out
		player_0_pos_modifier->addValue(5.0f, 0);
		track->addObjectModifier(player_0_obj, player_0_pos_modifier);

		// Slide Player 1 label in from right
		auto player_1_pos_modifier = AnimationModifierInt::create("widget.position_x");
		player_1_pos_modifier->addValue(0.0f, 1225);	// Off-screen right
		player_1_pos_modifier->addValue(0.25f, 975);	// Slide in
		player_1_pos_modifier->addValue(2.75f, 975);
		player_1_pos_modifier->addValue(3.0f, 1225);	// Slide out
		player_1_pos_modifier->addValue(5.0f, 1225);
		track->addObjectModifier(player_1_obj, player_1_pos_modifier);

		// Fade in/out both player labels (alpha component)
		auto player_color_modifier = AnimationModifierFloat::create("widget.font_color_w");
		player_color_modifier->addValue(0.0f, 0.001f);
		player_color_modifier->addValue(0.25f, 1.0f);
		player_color_modifier->addValue(2.75f, 1.0f);
		player_color_modifier->addValue(3.0f, 0.001f);
		player_color_modifier->addValue(5.0f, 0.001f);
		// Same modifier can be applied to multiple objects
		track->addObjectModifier(player_0_obj, player_color_modifier);
		track->addObjectModifier(player_1_obj, player_color_modifier);

		// Font size animation (scale effect)
		auto player_font_size_modifier = AnimationModifierInt::create("widget.font_size");
		player_font_size_modifier->addValue(0.0f, 80);
		player_font_size_modifier->addValue(0.25f, 100);
		player_font_size_modifier->addValue(2.75f, 100);
		player_font_size_modifier->addValue(3.0f, 80);
		player_font_size_modifier->addValue(5.0f, 80);
		track->addObjectModifier(player_0_obj, player_font_size_modifier);
		track->addObjectModifier(player_1_obj, player_font_size_modifier);

		// "vs" label: color cycling animation (cyan -> yellow -> cyan)
		auto vs_color_modifier = AnimationModifierFVec4::create("widget.font_color");
		vs_color_modifier->addValue(0.0f, vec4(0.0f, 1.0f, 1.0f, 0.001f));
		vs_color_modifier->addValue(0.25f, vec4(0.0f, 1.0f, 1.0f, 0.001f));
		vs_color_modifier->addValue(0.3f, vec4(0.0f, 1.0f, 1.0f, 1.0f));	// Fade in
		vs_color_modifier->addValue(1.5f, vec4(1.0f, 1.0f, 0.0f, 1.0f));	// Shift to yellow
		vs_color_modifier->addValue(2.70f, vec4(0.0f, 1.0f, 1.0f, 1.0f));	// Back to cyan
		vs_color_modifier->addValue(2.75f, vec4(0.0f, 1.0f, 1.0f, 0.001f));	// Fade out
		vs_color_modifier->addValue(3.0f, vec4(0.0f, 1.0f, 1.0f, 0.001f));
		vs_color_modifier->addValue(5.0f, vec4(0.0f, 1.0f, 1.0f, 0.001f));
		track->addObjectModifier(vs_obj, vs_color_modifier);

		playback = AnimationPlayback::create();
		playback->setTrack(track);
		playback->setLoop(true);
	}

	// ========================================================================================

	AnimationPlaybackPtr playback;
	WidgetPtr widgets;
};

REGISTER_COMPONENT(WidgetAnimationSample);
