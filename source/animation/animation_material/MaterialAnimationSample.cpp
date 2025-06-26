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

class MaterialAnimationSample : public ComponentBase
{
public:
	COMPONENT_DEFINE(MaterialAnimationSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init()
	{
		create_animations();

		// create object for animation
		box = Primitives::createBox(vec3(1.0f, 1.0f, 1.0f));
		box->setID(123);
		box->setName("box");
		box->setWorldPosition(Vec3(0.f, 0.f, 1.15f));

		// assign a material containing the "emission_scale" and "emission_color" parameters
		MaterialPtr mat = Materials::findMaterialByPath(joinPaths(getWorldRootPath(), "materials", "emission_mat.mat"));
		box->setMaterial(mat, "*");

		playback->play();

		gui.init(this);
	}

	void update()
	{
		gui.update();
	}

	void shutdown()
	{
		gui.shutdown();

		playback->stop();
	}

	void create_animations()
	{
		// get the index of the emission_scale parameter in our material
		MaterialPtr mat = Materials::findMaterialByPath(joinPaths(getWorldRootPath(), "materials", "emission_mat.mat"));
		int emission_scale_index = mat->findParameter("emission_scale");

		// create new track
		AnimationTrackPtr track = AnimationTrack::create();

		// create material animation object and add it to track
		AnimationObjectMaterialPtr anim_obj = AnimationObjectMaterial::create("mat");
		track->addObject(anim_obj);

		AnimationBindMaterialPtr bind = anim_obj->getBind();

		if (1) // bind the animation object to our object
		{
			bind->setAccess(AnimationBindMaterial::ACCESS_FROM_SURFACE);
			bind->setSurfaceDescription("dynamic", 0);
			bind->setObjectDescription(123, "box");
		}

		else // alternatively we can bind it to the material itself
		{
			bind->setAccess(AnimationBindMaterial::ACCESS_FROM_ASSET);
			bind->setMaterialDescription(mat->getGUID(), FileSystem::getGUID(joinPaths(getWorldRootPath(), "materials", "emission_mat.mat")));
		}

		anim_obj->setBind(bind);

		// animate emission scale by index
		auto scale_modifier = AnimationModifierFloat::create("material.parameter_float_indexed", emission_scale_index);
		scale_modifier->addValue(0.0f, 0.0f);
		scale_modifier->addValue(6.0f, 1.0f);
		scale_modifier->addValue(12.0f, 0.0f);
		track->addObjectModifier(anim_obj, scale_modifier);

		// animate emission color by name
		auto color_modifier = AnimationModifierFVec4::create("material.parameter_float4_named", "emission_color");
		color_modifier->addValue(0.0f, vec4_red);
		color_modifier->addValue(4.0f, vec4_green);
		color_modifier->addValue(8.0f, vec4_blue);
		color_modifier->addValue(12.0f, vec4_red);
		track->addObjectModifier(anim_obj, color_modifier);

		// create playback for track
		playback = AnimationPlayback::create();
		playback->setTrack(track);
		playback->setLoop(true);
	}

	// ========================================================================================

	struct SampleGui
	{
		void init(MaterialAnimationSample *sample)
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

			emission_scale = add_parameter(gridbox, "emission_scale");
			emission_color_r = add_parameter(gridbox, "emission_color.r");
			emission_color_g = add_parameter(gridbox, "emission_color.g");
			emission_color_b = add_parameter(gridbox, "emission_color.b");
		}

		void update()
		{
			auto material = sample->box->getMaterial(0);

			if (material)
			{
				emission_scale->setText(String::format("%.2f", material->getParameterFloat("emission_scale")));

				auto color = material->getParameterFloat4("emission_color");
				emission_color_r->setText(String::format("%.2f", color.x));
				emission_color_g->setText(String::format("%.2f", color.y));
				emission_color_b->setText(String::format("%.2f", color.z));
			}
		}

		void shutdown() { sample_description_window.shutdown(); }

		MaterialAnimationSample *sample = nullptr;
		SampleDescriptionWindow sample_description_window;
		WidgetEditLinePtr emission_scale;
		WidgetEditLinePtr emission_color_r;
		WidgetEditLinePtr emission_color_g;
		WidgetEditLinePtr emission_color_b;
	};

	AnimationPlaybackPtr playback;
	ObjectPtr box;
	SampleGui gui;
};

REGISTER_COMPONENT(MaterialAnimationSample);
