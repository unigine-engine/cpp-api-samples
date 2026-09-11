# Bones: Constraints [Animation Graph]

This sample demonstrates the use of joint rotation limits in an *animation graph* and illustrates how they affect the operation of inverse kinematics.

The target of the chain is moved with a manipulator, and the *IK Chain* node bends the bones of the left leg towards it. The rotation of each joint is limited, so the chain can only bend the way a real joint would, instead of taking any pose that reaches the target.

Every joint has its own limit node, with the joint, the axes, and the angles set in its parameters. A *Joint Limit Set* node gathers them all into a single set that is applied to the chain.

Rotation constraints are useful for keeping automatically generated poses plausible, for example, to prevent an elbow or a knee from bending backwards.