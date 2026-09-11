# Widget Animation

This sample demonstrates animating widgets using *AnimationBindRuntime*, a bind that stores the runtime instance of a target object. It can point an animation channel at any Engine object, such as a widget, a body, or a camera.

A looped title animation is played on three labels: the **Player 0** and **Player 1** labels are animated by position, font color, and font size, while the **vs** label between them is animated by font color only.

A channel animates a single parameter, so a label animated by several parameters at once needs a separate channel for each of them, with the same bind assigned to all of them.

Animating interface elements programmatically helps create more lively, appealing user interfaces and enables automation of their behavior.