# Global Engine Parameters Animation

This sample demonstrates animating global Engine parameters using **singleton channels** - animation channels that have no bind and target a global system instead of a certain node, material, or property.

Two parameters are animated at the same time by a single *AnimationSequence*: the Z component of physics gravity and the alpha component of the render background color. Their current values are displayed in the *State* window.

The sequence is saved to a `.seq` file in the `sequences` folder of the world and then played back right from this file, demonstrating a serialization roundtrip. If the file is unavailable, the sequence stored in memory is played instead.

Animating global parameters gives you programmatic control over scene effects, weather changes, time transitions, and physical properties, resulting in more engaging and interactive scenes.

> [!NOTE]
> All of the animation sequences, players and objects are managed by the animation system and have the "Engine lifetime", i.e. they exist from the point they're loaded or created to when the Engine shutdowns, and therefore are preserved between different worlds.
> You have to stop active players when switching to another world, if you don't want them continue playing there.