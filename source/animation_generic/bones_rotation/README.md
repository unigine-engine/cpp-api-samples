# Bones: Rotation [Animation Graph]

This sample demonstrates how to combine skeletal animation playback with direct modification of bone transforms.

The turret plays an idle animation, over which the left and right shooting animations are blended additively by a state machine in the *animation graph*.

On top of that, the horizontal joint of the turret is rotated from code at a constant speed. The rotation is applied after the animation graph has written its pose, so that the played animation does not overwrite it.

Combining played animations with programmatic bone control is useful for turrets and other cases where part of a skeleton has to be driven by code rather than by a pre-made animation.