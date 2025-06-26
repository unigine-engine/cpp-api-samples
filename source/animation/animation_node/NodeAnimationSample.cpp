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

class NodeAnimationSample : public ComponentBase
{
public:
	COMPONENT_DEFINE(NodeAnimationSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init()
	{
		create_animations();

		// create object for animation
		box = Primitives::createBox(Math::vec3_one);
		box->setName("box");
		box->setID(123);
		box->setWorldPosition(Vec3(0.f, 0.f, 1.15f));

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
		// create new track
		AnimationTrackPtr track = AnimationTrack::create();

		// create animation object and add it to track
		AnimationObjectNodePtr anim_obj = AnimationObjectNode::create("box");
		track->addObject(anim_obj);

		AnimationBindNodePtr bind = anim_obj->getBind();
		bind->setNodeDescription(123, "box");
		anim_obj->setBind(bind);

		// animate position of object
		auto position_modifier = AnimationModifierScalar::create("node.world_position_z");

		// create an animation curve explicitely
		auto position_curve = AnimationCurveScalar::create();
		position_curve->addKey(0.0f, 1.5f + 0.0f);
		position_curve->addKey(4.0f, 1.5f + 2.0f);
		position_curve->addKey(8.0f, 1.5f + 0.0f);

		// set all key types to KEY_TYPE_SMOOTH to get a bezier curve with symmetric tangents ((-1, 0) and (1, 0) by default)
		position_curve->setTypeOfAllKeys(AnimationCurve::KEY_TYPE_SMOOTH);

		// set the curve in the position modifier
		position_modifier->setCurve(position_curve);

		// add the position modifier to track
		track->addObjectModifier(anim_obj, position_modifier);

		// animate rotation of object
		auto rotation_modifier = AnimationModifierQuat::create(AnimationModifierQuat::MODE_QUAT, "node.world_rotation");
		rotation_modifier->addQuatValue(0.0f, quat(0.0f, 0.0f, 0.0f));
		rotation_modifier->addQuatValue(4.0f, quat(0.0f, 0.0f, 180.0f));
		rotation_modifier->addQuatValue(8.0f, quat(0.0f, 0.0f, 360.0f));
		track->addObjectModifier(anim_obj, rotation_modifier);

		// animate scale of object
		auto scale_modifier = AnimationModifierFVec3::create("node.world_scale");
		scale_modifier->addValue(0.0f, vec3(1.f, 1.f, 1.f), AnimationCurve::KEY_TYPE_SMOOTH);
		scale_modifier->addValue(4.0f, vec3(1.5f, 1.5f, 0.66f), AnimationCurve::KEY_TYPE_SMOOTH);
		scale_modifier->addValue(8.0f, vec3(1.f, 1.f, 1.f), AnimationCurve::KEY_TYPE_SMOOTH);
		track->addObjectModifier(anim_obj, scale_modifier);

		// create playback for track
		playback = AnimationPlayback::create();
		playback->setTrack(track);
		playback->setLoop(true);
	}

	// ========================================================================================

	struct SampleGui
	{
		void init(NodeAnimationSample *sample)
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

			node_position_z = add_parameter(gridbox, "node.position.z");
			node_rotation_z = add_parameter(gridbox, "node.rotation.z");
			node_scale_x = add_parameter(gridbox, "node.scale.x");
			node_scale_y = add_parameter(gridbox, "node.scale.y");
			node_scale_z = add_parameter(gridbox, "node.scale.z");
		}

		void update()
		{
			node_position_z->setText(String::format("%.2f", sample->box->getWorldPosition().z));
			node_rotation_z->setText(String::format("%.2f", sample->box->getWorldRotation().getAngle(vec3_up)));
			node_scale_x->setText(String::format("%.2f", sample->box->getWorldScale().x));
			node_scale_y->setText(String::format("%.2f", sample->box->getWorldScale().y));
			node_scale_z->setText(String::format("%.2f", sample->box->getWorldScale().z));
		}

		void shutdown() { sample_description_window.shutdown(); }

		SampleDescriptionWindow sample_description_window;
		NodeAnimationSample *sample = nullptr;
		WidgetEditLinePtr node_position_z;
		WidgetEditLinePtr node_rotation_z;
		WidgetEditLinePtr node_scale_x;
		WidgetEditLinePtr node_scale_y;
		WidgetEditLinePtr node_scale_z;
	};

	AnimationPlaybackPtr playback;
	NodePtr box;
	SampleGui gui;
};

REGISTER_COMPONENT(NodeAnimationSample);
