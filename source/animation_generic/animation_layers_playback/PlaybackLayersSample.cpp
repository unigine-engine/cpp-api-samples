#include "PlaybackLayersSample.h"

#include <UnigineEngine.h>
#include <UnigineLogic.h>
#include <UnigineWorld.h>
#include <UniginePrimitives.h>

REGISTER_COMPONENT(PlaybackLayersSample);

using namespace Unigine;
using namespace Math;

// Animations are created and both players are started.
void PlaybackLayersSample::init()
{
	create_animations();

	// Create objects for animation. The channels animate local transform parameters,
	// so a parent node places its cube in the scene without disturbing the animation.
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

	player_combined->play();
	player_sequential->play();
}

// Both players are stopped on component destruction.
void PlaybackLayersSample::shutdown()
{
	player_combined->stop();
	player_sequential->stop();
}

// Three single-parameter sequences are created and combined in two different ways.
void PlaybackLayersSample::create_animations()
{
	// Piece 1: Z position bounce
	position_sequence = AnimationSequence::create();

	AnimationChannelScalarPtr position_channel = AnimationChannelScalar::create("node.position_z");
	position_channel->setBind(AnimationBindNode::create());
	position_channel->addValue(0.0f, 0.0f, AnimationCurve::KEY_TYPE_SMOOTH);
	position_channel->addValue(3.0f, 2.0f, AnimationCurve::KEY_TYPE_SMOOTH);
	position_channel->addValue(6.0f, 0.0f, AnimationCurve::KEY_TYPE_SMOOTH);
	position_sequence->addChannel(position_channel);

	// Piece 2: Z-axis rotation
	rotation_sequence = AnimationSequence::create();

	AnimationChannelQuatPtr rotation_channel = AnimationChannelQuat::create(AnimationChannelQuat::MODE_QUAT, "node.rotation");
	rotation_channel->setBind(AnimationBindNode::create());
	rotation_channel->addQuatValue(0.0f, quat(0.0f, 0.0f, 0.0f));
	rotation_channel->addQuatValue(3.0f, quat(0.0f, 0.f, 180.0f));
	rotation_channel->addQuatValue(6.0f, quat(0.0f, 0.0f, 360.0f));
	rotation_sequence->addChannel(rotation_channel);

	// Piece 3: Scale pulse
	scale_sequence = AnimationSequence::create();

	AnimationChannelFVec3Ptr scale_channel = AnimationChannelFVec3::create("node.scale");
	scale_channel->setBind(AnimationBindNode::create());
	scale_channel->addValue(0.0f, vec3(1.f, 1.f, 1.f), AnimationCurve::KEY_TYPE_SMOOTH);
	scale_channel->addValue(3.0f, vec3(1.5f, 1.5f, 0.66f), AnimationCurve::KEY_TYPE_SMOOTH);
	scale_channel->addValue(6.0f, vec3(1.f, 1.f, 1.f), AnimationCurve::KEY_TYPE_SMOOTH);
	scale_sequence->addChannel(scale_channel);

	// Combined: the three clips cover the same time range, so they play in parallel
	{
		combined_sequence = AnimationSequence::create();

		AnimationChannelSubSequencePtr clips = AnimationChannelSubSequence::create();
		clips->addEmbeddedSubSequence(0.0f, PIECE_DURATION, position_sequence);
		clips->addEmbeddedSubSequence(0.0f, PIECE_DURATION, rotation_sequence);
		clips->addEmbeddedSubSequence(0.0f, PIECE_DURATION, scale_sequence);
		combined_sequence->addChannel(clips);

		player_combined = AnimationSequencePlayer::create(combined_sequence);
		player_combined->setLoop(true);

		// This player drives the left cube
		retarget_player(player_combined, 123, "box_node_combined");
	}

	// Sequential: the clips follow each other, so the pieces play one after another.
	// A finished clip stops writing its parameter, and the cube keeps the value it left.
	{
		sequential_sequence = AnimationSequence::create();

		AnimationChannelSubSequencePtr clips = AnimationChannelSubSequence::create();
		clips->addEmbeddedSubSequence(0.0f, PIECE_DURATION, position_sequence);
		clips->addEmbeddedSubSequence(PIECE_DURATION, PIECE_DURATION, rotation_sequence);
		clips->addEmbeddedSubSequence(PIECE_DURATION * 2.0f, PIECE_DURATION, scale_sequence);
		sequential_sequence->addChannel(clips);

		player_sequential = AnimationSequencePlayer::create(sequential_sequence);
		player_sequential->setLoop(true);

		// The right cube is the target of this player
		retarget_player(player_sequential, 456, "box_node_sequential");
	}
}

// A player enumerates every bind of its sequences, including the ones inside sub-sequences,
// so the shared pieces are pointed at this player's own node without being changed themselves.
void PlaybackLayersSample::retarget_player(const AnimationSequencePlayerPtr &player, int node_id, const char *node_name)
{
	AnimationBindNodePtr bind = AnimationBindNode::create();
	bind->setNumTargets(1);
	bind->setTargetNodeDescription(0, node_id, node_name);

	for (int i = 0; i < player->getBindCount(); i += 1)
		player->setBind(i, bind);
}
