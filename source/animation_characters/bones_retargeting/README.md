# Bones: Retargeting [Animation Graph]

This sample demonstrates how the same animation can be used on skeletons with different proportions.

Both pairs play the same walking animation through an *animation graph*. The pair on the left plays it as is, so the child inherits the proportions of the adult and appears stretched. The pair on the right plays the retargeted version, where the child keeps its own proportions.

Retargeting works automatically as long as the skeleton is shared between the skinned mesh and the animation. To prevent stretching when the proportions differ, rotation-only masks are applied to most of the bones, while the hip and the ground-contact bones receive full transformations to keep the body height and the placement on the ground correct.

Retargeting is useful when you need to use the same animation source for different characters or objects with varying proportions but similar skeletal structures. This approach significantly speeds up the process of preparing animations.