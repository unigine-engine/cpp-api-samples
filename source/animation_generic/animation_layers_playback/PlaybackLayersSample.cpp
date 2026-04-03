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

// This component demonstrates animation layers for combining multiple tracks.
// Two playback modes are shown:
// - Combined: tracks on different layers play simultaneously (position + rotation + scale at once)
// - Sequential: tracks on the same layer play one after another (position, then rotation, then scale)
// The same animation object can have different bindings per playback using setPlaybackOverriddenBind().
class PlaybackLayersSample : public ComponentBase
{
public:
	COMPONENT_DEFINE(PlaybackLayersSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	// Animation objects are created and both playback modes are started.
	void init()
	{
		create_animations();

		// Create objects for animation
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

	// Both playbacks are stopped on component destruction.
	void shutdown()
	{
		playback_combined->stop();
		playback_sequential->stop();
	}

	// Three separate tracks are created and combined in two different playback modes.
	void create_animations()
	{
		// Single animation object is shared by all tracks
		AnimationObjectNodePtr anim_obj = AnimationObjectNode::create("box");

		// Track 1: Z position bounce
		AnimationTrackPtr position_track = AnimationTrack::create();
		position_track->addObject(anim_obj);

		auto position_modifier = AnimationModifierScalar::create("node.position_z");
		position_modifier->addValue(0.0f, 0.0f, AnimationCurve::KEY_TYPE_SMOOTH);
		position_modifier->addValue(3.0f, 2.0f, AnimationCurve::KEY_TYPE_SMOOTH);
		position_modifier->addValue(6.0f, 0.0f, AnimationCurve::KEY_TYPE_SMOOTH);
		position_track->addObjectModifier(anim_obj, position_modifier);

		// Track 2: Z-axis rotation
		AnimationTrackPtr rotation_track = AnimationTrack::create();
		rotation_track->addObject(anim_obj);

		auto rotation_modifier = AnimationModifierQuat::create(AnimationModifierQuat::MODE_QUAT, "node.rotation");
		rotation_modifier->addQuatValue(0.0f, quat(0.0f, 0.0f, 0.0f));
		rotation_modifier->addQuatValue(3.0f, quat(0.0f, 0.f, 180.0f));
		rotation_modifier->addQuatValue(6.0f, quat(0.0f, 0.0f, 360.0f));
		rotation_track->addObjectModifier(anim_obj, rotation_modifier);

		// Track 3: Scale pulse
		AnimationTrackPtr scale_track = AnimationTrack::create();
		scale_track->addObject(anim_obj);

		auto scale_modifier = AnimationModifierFVec3::create("node.scale");
		scale_modifier->addValue(0.0f, vec3(1.f, 1.f, 1.f), AnimationCurve::KEY_TYPE_SMOOTH);
		scale_modifier->addValue(3.0f, vec3(1.5f, 1.5f, 0.66f), AnimationCurve::KEY_TYPE_SMOOTH);
		scale_modifier->addValue(6.0f, vec3(1.f, 1.f, 1.f), AnimationCurve::KEY_TYPE_SMOOTH);
		scale_track->addObjectModifier(anim_obj, scale_modifier);

		// Combined playback: all tracks play at the same time on different layers
		{
			playback_combined = AnimationPlayback::create();
			playback_combined->setLoop(true);

			// addLayer() creates a new layer for each track - they play in parallel
			playback_combined->setLayerTrack(0, 0, position_track);
			playback_combined->addLayer(rotation_track);	// Layer 1
			playback_combined->addLayer(scale_track);		// Layer 2

			// Override binding for this specific playback (left cube)
			AnimationBindNodePtr bind = AnimationBindNode::create();
			bind->setNodeDescription(123, "box_node_combined");
			anim_obj->setPlaybackOverriddenBind(playback_combined, bind);
		}

		// Sequential playback: tracks play one after another on the same layer
		{
			playback_sequential = AnimationPlayback::create();
			playback_sequential->setLoop(true);

			// addLayerTrack() adds tracks to same layer - they play sequentially
			playback_sequential->setLayerTrack(0, 0, position_track);
			playback_sequential->addLayerTrack(0, rotation_track);
			playback_sequential->addLayerTrack(0, scale_track);

			// ApplyBefore/After ensures animation persists between track switches
			for (int i = 0; i < playback_sequential->getNumLayerTracks(0); i++)
			{
				playback_sequential->setLayerTrackApplyBefore(0, i, true);
				playback_sequential->setLayerTrackApplyAfter(0, i, true);
			}

			// Override binding for this specific playback (right cube)
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
