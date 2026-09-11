# Node Parameters Animation

This sample demonstrates animating the transformation of a node using **node binds** - *AnimationBindNode* objects that point animation channels at a certain node, identified by its ID and name.

A box is animated by three channels of different types added to a single *AnimationSequence*, all of them sharing the same bind:

-*AnimationChannelScalar* - animates a single float value, here the Z position of the box.
-*AnimationChannelQuat* - animates rotation via quaternion interpolation. Along with the quaternion mode used here, the rotation can also be composed of three separate curves for each of the angles.
-*AnimationChannelFVec3* - animates a three-component vector, here the scale of the box.

The position channel gets its keyframes from an *AnimationCurveScalar* curve created explicitly, while the other two channels take the values directly. The keys use smooth interpolation based on a Bezier curve, so the box moves and resizes with easing instead of linearly.

The current values of the animated parameters are displayed in the *State* window.

Node animations can serve as a foundation for various effects, including element appearance/disappearance, rotations, resizing, and more.