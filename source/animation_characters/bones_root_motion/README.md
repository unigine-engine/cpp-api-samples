# Bones: Root Motion [Animation Graph]

This sample demonstrates the implementation of the **root motion** technique, where the movement stored in the root bone of an animation is applied to the object itself instead of being played in place.

Both characters play the same walking animation through an *animation graph*, and the only difference between the two graphs is the **Root Motion** option. The character on the left keeps walking in place, while the one on the right is carried along the path of the animation. The coordinate axes drawn at the position of each character show the difference.

The movement accumulated by the animation is read every frame as a delta transformation and applied to the world transformation of the node after the animation has been evaluated.

Root motion is particularly valuable for realistic character, vehicle, or object movements in games and simulations, as the object goes exactly where the animation takes it, with no foot skating.