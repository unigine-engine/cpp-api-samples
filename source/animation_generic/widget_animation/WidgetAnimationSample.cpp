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

// This component demonstrates widget animation using AnimationBindRuntime.
// Unlike node/material binds, a runtime bind can target any engine object
// (widgets, bodies, cameras) by pointing directly at the instance at runtime.
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
		// Widgets come first: a runtime bind points at an existing instance
		create_widgets();

		create_animations();

		player->play();
	}

	void shutdown()
	{
		widgets.deleteLater();

		player->stop();
	}

	// Create the "Player 0 vs Player 1" title widgets
	void create_widgets()
	{
		EngineWindowViewportPtr main_window = WindowManager::getMainWindow();
		widgets = WidgetVBox::create();

		player_0_label = WidgetLabel::create("Player 0");
		player_0_label->setFontOutline(1);
		player_0_label->setFontSize(100);
		player_0_label->setPosition(250, 300);
		player_0_label->setFontColor(vec4_red);
		widgets->addChild(player_0_label, Gui::ALIGN_OVERLAP);

		player_1_label = WidgetLabel::create("Player 1");
		player_1_label->setFontOutline(1);
		player_1_label->setFontSize(100);
		player_1_label->setPosition(975, 500);
		player_1_label->setFontColor(vec4_green);
		widgets->addChild(player_1_label, Gui::ALIGN_OVERLAP);

		vs_label = WidgetLabel::create("vs");
		vs_label->setFontOutline(1);
		vs_label->setFontSize(100);
		vs_label->setPosition(750, 400);
		widgets->addChild(vs_label, Gui::ALIGN_OVERLAP);

		main_window->addChild(widgets, Gui::ALIGN_OVERLAP);
	}

	// Create animation sequence for the title animation
	void create_animations()
	{
		sequence = AnimationSequence::create();

		// Both player labels are animated the same way, they only slide in from opposite sides
		add_player_label_channels(player_0_label, 0, 250);		// Off-screen left, then in place
		add_player_label_channels(player_1_label, 1225, 975);	// Off-screen right, then in place

		// "vs" label: color cycling animation (cyan -> yellow -> cyan)
		AnimationChannelFVec4Ptr vs_color_channel = AnimationChannelFVec4::create("widget.font_color");
		vs_color_channel->setBind(create_widget_bind(vs_label));
		vs_color_channel->addValue(0.0f, vec4(0.0f, 1.0f, 1.0f, 0.001f));
		vs_color_channel->addValue(0.25f, vec4(0.0f, 1.0f, 1.0f, 0.001f));
		vs_color_channel->addValue(0.3f, vec4(0.0f, 1.0f, 1.0f, 1.0f));		// Fade in
		vs_color_channel->addValue(1.5f, vec4(1.0f, 1.0f, 0.0f, 1.0f));		// Shift to yellow
		vs_color_channel->addValue(2.70f, vec4(0.0f, 1.0f, 1.0f, 1.0f));	// Back to cyan
		vs_color_channel->addValue(2.75f, vec4(0.0f, 1.0f, 1.0f, 0.001f));	// Fade out
		vs_color_channel->addValue(3.0f, vec4(0.0f, 1.0f, 1.0f, 0.001f));
		vs_color_channel->addValue(5.0f, vec4(0.0f, 1.0f, 1.0f, 0.001f));
		sequence->addChannel(vs_color_channel);

		player = AnimationSequencePlayer::create(sequence);
		player->setLoop(true);
	}

	// Runtime binding: the bind holds the widget instance itself, not an asset description
	AnimationBindRuntimePtr create_widget_bind(const WidgetPtr &widget)
	{
		AnimationBindRuntimePtr bind = AnimationBindRuntime::create();
		bind->setWidget(widget);

		return bind;
	}

	// Slide, fade and scale one of the player labels. Every channel carries its own bind,
	// so a label animated by several parameters needs a channel per parameter.
	void add_player_label_channels(const WidgetPtr &label, int hidden_position_x, int visible_position_x)
	{
		AnimationBindRuntimePtr bind = create_widget_bind(label);

		// Slide the label in and back out
		AnimationChannelIntPtr position_channel = AnimationChannelInt::create("widget.position_x");
		position_channel->setBind(bind);
		position_channel->addValue(0.0f, hidden_position_x);
		position_channel->addValue(0.25f, visible_position_x);	// Slide in
		position_channel->addValue(2.75f, visible_position_x);	// Hold
		position_channel->addValue(3.0f, hidden_position_x);		// Slide out
		position_channel->addValue(5.0f, hidden_position_x);
		sequence->addChannel(position_channel);

		// Fade the label in and out (alpha component)
		AnimationChannelFloatPtr color_channel = AnimationChannelFloat::create("widget.font_color_w");
		color_channel->setBind(bind);
		color_channel->addValue(0.0f, 0.001f);
		color_channel->addValue(0.25f, 1.0f);
		color_channel->addValue(2.75f, 1.0f);
		color_channel->addValue(3.0f, 0.001f);
		color_channel->addValue(5.0f, 0.001f);
		sequence->addChannel(color_channel);

		// Font size animation (scale effect)
		AnimationChannelIntPtr font_size_channel = AnimationChannelInt::create("widget.font_size");
		font_size_channel->setBind(bind);
		font_size_channel->addValue(0.0f, 80);
		font_size_channel->addValue(0.25f, 100);
		font_size_channel->addValue(2.75f, 100);
		font_size_channel->addValue(3.0f, 80);
		font_size_channel->addValue(5.0f, 80);
		sequence->addChannel(font_size_channel);
	}

	// ========================================================================================

	// A player does not own a sequence built in code, so the sample keeps it alive itself
	AnimationSequencePtr sequence;
	AnimationSequencePlayerPtr player;

	WidgetPtr widgets;
	WidgetPtr player_0_label;
	WidgetPtr player_1_label;
	WidgetPtr vs_label;
};

REGISTER_COMPONENT(WidgetAnimationSample);
