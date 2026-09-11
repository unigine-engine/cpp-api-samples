# Curve2D Animation

This sample demonstrates animating node transformations and material parameters with *Curve2D* values sampled every frame.

Two components are shown:

-**CurveAnimationTransform** - animates the transformation of a node. Position, rotation, and scale have a separate curve for each of the **X, Y, Z** axes, so every axis can be shaped independently. The curves are evaluated at the current time and composed into the final transformation matrix. In the sample it moves the platforms, and changes the coins and heart transformations.
-**CurveAnimationMaterialParamFloat** - animates float parameters of a material. Each entry of the list binds a curve to a parameter by its name and surface index, so any number of parameters can be animated at once. In the sample it pulses the emission of the coins and the heart.

The curves and the playback speed are available in the properties of the corresponding component in UnigineEditor, where the curves can also be shaped in the *Curve Editor*.

This setup is useful for looping motions and dynamic material effects that do not require external animation assets.