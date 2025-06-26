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
		// create tracks and playback
		create_animations();

		// create object for animation
		box = Primitives::createBox(vec3(1.0f, 1.0f, 1.0f));
		box->setID(123);
		box->setName("box");
		box->setWorldPosition(Vec3(0.f, 0.f, 1.15f));

		PropertyPtr prop = Properties::findPropertyByPath(joinPaths(getWorldRootPath(), "properties", "speed_prop.prop"));
		box->addProperty(prop);

		playback->play();

		gui.init(this);
	}

	void update()
	{
		PropertyPtr prop = box->getProperty();
		float speed = prop->getParameterPtr("speed")->getValueFloat();

		box->worldRotate(0.f, 0.f, speed * Game::getIFps());

		gui.update();
	}

	void shutdown()
	{
		gui.shutdown();
		playback->stop();
	}

	void create_animations()
	{
		// create new track
		AnimationTrackPtr track = AnimationTrack::create();

		// create property parameter animation object and add it to the track
		AnimationObjectPropertyParameterPtr anim_obj = AnimationObjectPropertyParameter::create("param");
		track->addObject(anim_obj);

		// bind the animation object to our node and property
		AnimationBindPropertyParameterPtr bind = anim_obj->getBind();
		bind->setAccess(AnimationBindPropertyParameter::ACCESS_FROM_NODE);
		bind->setNodePropertyDescription("speed_prop", 0);
		bind->setNodeDescription(123, "box");
		bind->setParameterPath("speed");
		anim_obj->setBind(bind);

		// animate speed value
		AnimationModifierFloatPtr param_modifier = AnimationModifierFloat::create("property_parameter.value_float");
		param_modifier->addValue(0.0f, 0.0f);
		param_modifier->addValue(3.0f, 120.0f);
		param_modifier->addValue(9.0f, -120.0f);
		param_modifier->addValue(12.0f, 0.0f);
		track->addObjectModifier(anim_obj, param_modifier);

		// create playback for track
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
