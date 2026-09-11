# Bones: State Machine [Animation Graph]

This sample demonstrates how to build animation state machines in an *animation graph*. A state machine manages various object states - such as idle, walking, or running - and handles transitions between them.

Three characters show different configurations, each with its own graph:

-**Walk** (left) - transitions between idle and walking depending on the speed value, with a separate turn-around animation triggered by a flag.
-**Turn** (center) - a one-dimensional *blend space*, where a single value between -1 and 1 blends between turning left, standing still, and turning right.
-**Run** (right) - a two-dimensional blend space, where one value blends idle, walking, and running, and the other one blends turning to the left and to the right.

The component itself only sets the named parameters of each graph from the keyboard input, while all the states, transitions, and blending are configured in the graph assets. Every state machine also uses root motion, so the characters are carried by the animations they play.

Implementing state machines enables the creation of complex, flexible character behaviors.