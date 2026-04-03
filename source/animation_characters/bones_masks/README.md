# Bones: Masks

This sample demonstrates how to use bone masks to selectively apply animation data.

Masks enable complex animation combinations. For example, you can preserve the original body movements while limiting head or arm animations to rotation, or scaling specific bones.

In this example, the right (child) model has a component that lists bones using a rotation-only mask from the left (adult) model's animation. The scale is masked out, so the listed bones keep their original size. If the mask is not applied, the bones appear stretched, as all transformations (including scale) are copied from the adult model. This can be seen on the child model's arms, where the mask is not used.

This method is excellent for multi-layered animations, allowing you to mix motions, add effects, or exclude certain parts of the model to achieve specific visual results or behaviors without duplicating full animation sets.