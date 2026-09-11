# Animation Layers Playback

This sample demonstrates composing a single animation out of several reusable sequences using *AnimationChannelSubSequence* - a channel that holds other sequences as clips, with each clip having its own start time and duration.

Three single-parameter sequences are created first: a Z position bounce, a rotation around the Z axis, and a scale pulse. They are then combined in two different ways, and both results are played at the same time:

-The **left box** plays the clips starting at the same time, so its position, rotation, and scale are animated simultaneously.
-The **right box** plays the clips one after another. A clip that has finished stops writing its parameter, and the box keeps the value it was left with.

Both players use the same three sequences. Instead of duplicating them for each box, every player enumerates the binds of its sequences, including the ones inside the clips, and points them at its own box, so the sequences remain reusable and are not changed themselves.

Composing animations out of reusable pieces makes it possible to build complex animation scenarios from a small set of simple sequences and reuse them for any number of objects.