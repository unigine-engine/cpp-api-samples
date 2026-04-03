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

// This component demonstrates property parameter animation using the Animation system.
// AnimationObjectPropertyParameter allows animating any property parameter attached to a node.
// Here, a "speed" property parameter is animated, which then drives object rotation in update().
// This pattern is useful for data-driven animations where logic reads animated values.
class PropertyAnimationSample : public ComponentBase
{
public:
	COMPONENT_DEFINE(PropertyAnimationSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init()
	{
		// Animation tracks and playback are created
		create_animations();

		// A box primitive is created as the animation target
		box = Primitives::createBox(vec3(1.0f, 1.0f, 1.0f));
		box->setID(123);
		box->setName("box");
		box->setWorldPosition(Vec3(0.f, 0.f, 1.15f));

		PropertyPtr prop = Properties::findPropertyByPath(joinPaths(getWorldRootPath(), "properties", "speed_prop.prop"));
		box->addProperty(prop);

		playback->play();

		// GUI is initialized manually as SampleGui is a plain struct
		gui.init(this);
	}

	void update()
	{
		// The animated property value is read and used to drive rotation.
		// The animation system updates the property; game logic reads and applies it.
		PropertyPtr prop = box->getProperty();
		float speed = prop->getParameterPtr("speed")->getValueFloat();

		box->worldRotate(0.f, 0.f, speed * Game::getIFps());

		// GUI state is updated manually each frame
		gui.update();
	}

	void shutdown()
	{
		// GUI resources are released manually before component destruction
		gui.shutdown();
		playback->stop();
	}

	// Animation that modifies a property parameter value over time is created
	void create_animations()
	{
		AnimationTrackPtr track = AnimationTrack::create();

		// AnimationObjectPropertyParameter targets a specific parameter within a property
		AnimationObjectPropertyParameterPtr anim_obj = AnimationObjectPropertyParameter::create("param");
		track->addObject(anim_obj);

		// Configure binding: ACCESS_FROM_NODE finds the property on a specific node
		AnimationBindPropertyParameterPtr bind = anim_obj->getBind();
		bind->setAccess(AnimationBindPropertyParameter::ACCESS_FROM_NODE);
		bind->setNodePropertyDescription("speed_prop", 0);	// Property name and index
		bind->setNodeDescription(123, "box");				// Target node
		bind->setParameterPath("speed");					// Parameter name within property
		anim_obj->setBind(bind);

		// "property_parameter.value_float" is the modifier path for float property parameters
		AnimationModifierFloatPtr param_modifier = AnimationModifierFloat::create("property_parameter.value_float");
		param_modifier->addValue(0.0f, 0.0f);		// Start at 0
		param_modifier->addValue(3.0f, 120.0f);		// Accelerate to 120
		param_modifier->addValue(9.0f, -120.0f);	// Reverse to -120
		param_modifier->addValue(12.0f, 0.0f);		// Return to 0
		track->addObjectModifier(anim_obj, param_modifier);

		playback = AnimationPlayback::create();
		playback->setTrack(track);
		playback->setLoop(true);
	}

	// ========================================================================================

	struct SampleGui
	{
		void init(PropertyAnimationSample *sample)
		{
			this->sample = sample;

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

			speed = add_parameter(gridbox, "speed_prop.speed");
		}

		void update()
		{
			PropertyPtr prop = sample->box->getProperty();
			float prop_speed = prop->getParameterPtr("speed")->getValueFloat();

			speed->setText(String::format("%.2f", prop_speed));
		}

		void shutdown() { sample_description_window.shutdown(); }

		PropertyAnimationSample *sample = nullptr;
		SampleDescriptionWindow sample_description_window;
		WidgetEditLinePtr speed;
	};

	NodePtr box;
	AnimationPlaybackPtr playback;
	SampleGui gui;
};

REGISTER_COMPONENT(PropertyAnimationSample);
