#pragma once

#include <UnigineAnimation.h>
#include <UnigineComponentSystem.h>

// This component demonstrates composing one animation out of several reusable sequences.
// A sub-sequence channel holds them as clips, and clip times decide how they are combined:
// - Combined: the clips start together, so position, rotation and scale play at once
// - Sequential: the clips follow each other, so position, then rotation, then scale play
// The same sequences serve both players: each player retargets their binds to its own cube.
class PlaybackLayersSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(PlaybackLayersSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();

	void create_animations();
	void retarget_player(const Unigine::AnimationSequencePlayerPtr &player, int node_id, const char *node_name);

private:
	// Length of a single piece, in seconds: the keys of every piece end at this time
	static constexpr float PIECE_DURATION = 6.0f;

	// A player does not own a sequence built in code, so the sample keeps them all alive itself
	Unigine::AnimationSequencePtr position_sequence;
	Unigine::AnimationSequencePtr rotation_sequence;
	Unigine::AnimationSequencePtr scale_sequence;
	Unigine::AnimationSequencePtr combined_sequence;
	Unigine::AnimationSequencePtr sequential_sequence;

	Unigine::AnimationSequencePlayerPtr player_combined;
	Unigine::AnimationSequencePlayerPtr player_sequential;
};
