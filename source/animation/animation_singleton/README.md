# Singleton Animation

This sample demonstrates animating global engine parameters using **singleton animation modifiers**.
Animation modifiers give you programmatic control over scene effects, weather changes, time transitions, and the animation of physical properties, resulting in more engaging and interactive scenes.

> [!NOTE]
> All of the animation tracks, playbacks and objects are managed by the animation system and have the "engine lifetime", i.e. they exist from the point they're loaded or created to when the engine shutdowns, and therefore are preserved between different worlds.
> You have to stop active playbacks when switching to another world, if you don't want them continue playing there.