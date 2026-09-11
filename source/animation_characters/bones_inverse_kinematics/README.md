# Bones: Inverse Kinematics [Animation Graph]

This sample demonstrates how to control bones using inverse kinematics in an *animation graph*. Instead of rotating each bone individually, the *Two Bone IK* node takes the position the end of the chain has to reach and bends the bones towards it. Here the chain is the left leg of the character, with its three joints picked in the node's parameters.

The node takes the pose from the *Animation Player* node, solves the chain against the target, and passes the result on, so the idle keeps playing while the foot reaches for it. The lower manipulator moves that target, the upper one moves the pole vector that defines the plane the chain bends in.

The example is useful for creating animations where character bones are automatically positioned and oriented to achieve realistic movement.