# Bones: Look At Chains [Animation Graph]

This sample demonstrates the use of LookAt chains for aiming at a target in an *animation graph*: the *Look At Chain* node turns the bones of the chain automatically so that the character follows the target with its body.

The chain contains three spine bones, the neck, and the head. Each of them has its own weight defining how much it contributes to the resulting rotation - the weights grow towards the head, so the head turns the most and the body follows more subtly.

One manipulator moves the target the character looks at, the other one moves the pole vector that sets the upward direction for the bones of the chain.

The bones, their weights, and their axes are set in the parameters of the *Look At Chain* node.

LookAt chains enable control over the orientation of a character's body, making it react to the environment as it changes, such as an NPC following the player with its gaze, a driver turning to a road sign, and others.