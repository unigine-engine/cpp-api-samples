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

class PlaybackLayersSample : public ComponentBase
{
public:
	COMPONENT_DEFINE(PlaybackLayersSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init()
	{
		create_animations();

		// create objects for animation
		NodePtr parent_0 = NodeDummy::create();
		NodePtr parent_1 = NodeDummy::create();

		NodePtr box_node_combined = Primitives::createBox(Math::vec3_one);
		box_node_combined->setName("box_node_combined");
		box_node_combined->setID(123);
		box_node_combined->setParent(parent_0);
		parent_0->setWorldPosition(Vec3(-2.0f, 0.0f, 1.75f));

		NodePtr box_node_sequential = Primitives::createBox(Math::vec3_one);
		box_node_sequential->setName("box_node_sequential");
		box_node_sequential->setID(456);
		box_node_sequential->setParent(parent_1);
		parent_1->setWorldPosition(Vec3(2.0f, 0.0f, 1.75f));

		playback_combined->play();
		playback_sequential->play();
	}

	void shutdown()
	{
		playback_combined->stop();
		playback_sequential->stop();
	}

	void create_animations()
	{
		// create animation object
		AnimationObjectNodePtr anim_obj = AnimationObjectNode::create("box");

		// create position track
		AnimationTrackPtr position_track = AnimationTrack::create();
		position_track->addObject(anim_obj);

		auto position_modifier = AnimationModifierScalar::create("node.position_z");
		position_modifier->addValue(0.0f, 0.0f, AnimationCurve::KEY_TYPE_SMOOTH);
		position_modifier->addValue(3.0f, 2.0f, AnimationCurve::KEY_TYPE_SMOOTH);
		position_modifier->addValue(6.0f, 0.0f, AnimationCurve::KEY_TYPE_SMOOTH);
		position_track->addObjectModifier(anim_obj, position_modifier);

		// create rotation track
		AnimationTrackPtr rotation_track = AnimationTrack::create();
		rotation_track->addObject(anim_obj);

		auto rotation_modifier = AnimationModifierQuat::create(AnimationModifierQuat::MODE_QUAT, "node.rotation");
		rotation_modifier->addQuatValue(0.0f, quat(0.0f, 0.0f, 0.0f));
		rotation_modifier->addQuatValue(3.0f, quat(0.0f, 0.f, 180.0f));
		rotation_modifier->addQuatValue(6.0f, quat(0.0f, 0.0f, 360.0f));
		rotation_track->addObjectModifier(anim_obj, rotation_modifier);

		// create scale track
		AnimationTrackPtr scale_track = AnimationTrack::create();
		scale_track->addObject(anim_obj);

		auto scale_modifier = AnimationModifierFVec3::create("node.scale");
		scale_modifier->addValue(0.0f, vec3(1.f, 1.f, 1.f), AnimationCurve::KEY_TYPE_SMOOTH);
		scale_modifier->addValue(3.0f, vec3(1.5f, 1.5f, 0.66f), AnimationCurve::KEY_TYPE_SMOOTH);
		scale_modifier->addValue(6.0f, vec3(1.f, 1.f, 1.f), AnimationCurve::KEY_TYPE_SMOOTH);
		scale_track->addObjectModifier(anim_obj, scale_modifier);

		// create combined playback
		{
			playback_combined = AnimationPlayback::create();
			playback_combined->setLoop(true);

			// each track is added to a new layer and played simultaneously
			playback_combined->setLayerTrack(0, 0, position_track);
			playback_combined->addLayer(rotation_track);
			playback_combined->addLayer(scale_track);

			// bind the animation object to the left node for this combined playback
			AnimationBindNodePtr bind = AnimationBindNode::create();
			bind->setNodeDescription(123, "box_node_combined");
			anim_obj->setPlaybackOverriddenBind(playback_combined, bind);
		}

		// create sequential playback
		{
			playback_sequential = AnimationPlayback::create();
			playback_sequential->setLoop(true);

			// add tracks one after another on one layer
			playback_sequential->setLayerTrack(0, 0, position_track);
			playback_sequential->addLayerTrack(0, rotation_track);
			playback_sequential->addLayerTrack(0, scale_track);

			for (int i = 0; i < playback_sequential->getNumLayerTracks(0); i++)
			{
				playback_sequential->setLayerTrackApplyBefore(0, i, true);
				playback_sequential->setLayerTrackApplyAfter(0, i, true);
			}

			// bind the animation object to the right node for this sequential playback
			AnimationBindNodePtr bind = AnimationBindNode::create();
			bind->setNodeDescription(456, "box_node_sequential");
			anim_obj->setPlaybackOverriddenBind(playback_sequential, bind);
		}
	}

	// ========================================================================================

	AnimationPlaybackPtr playback_combined;
	AnimationPlaybackPtr playback_sequential;
};

REGISTER_COMPONENT(PlaybackLayersSample);
