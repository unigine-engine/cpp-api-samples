# Property Animation

This sample demonstrates animating property parameters using *AnimationBindPropertyParameter*, which points an animation channel at a certain parameter of a property assigned to a node.

A box has the **speed_prop** property assigned to it, with a single float parameter named **speed**. This parameter is animated by an *AnimationChannelFloat* channel: the value grows from **0** to **120**, then goes down to **-120**, and returns back to **0**.

The animation itself does not move the box. Each frame the sample reads the current value of the **speed** parameter and rotates the box at this speed. The current value is displayed in the *State* window.

This pattern is useful for data-driven animations, where the animation system controls the values, and the logic decides how to use them. The same animated parameter can be read by any number of components, and the property can be reassigned to another node without changing the animation.