# Tracker: Playback

This sample demonstrates playing back animation tracks created in the **Tracker** tool and stored in `.track` files.

Three tracks are played at the same time, animating the position, rotation, and scale of the object independently. The position and rotation tracks are listed in the properties of the **Tracker** component and loaded on its initialization, while the scale track is added later by the **TrackPlayback** component.

The tracks are addressed in two different ways: by their ID, cached once at initialization, and by their name, which is more readable but requires a lookup on every call.

Each track keeps its own playback time, advanced every frame and wrapped back to the beginning at the end of the track.

Tracker comes in handy for creating complex animation scenarios, where the animation is authored visually rather than in code.

The **Tracker.cpp** file provides a ready-to-use C++ interface to the **Tracker** system, which the **TrackPlayback** component works through. If you want to reuse the sample in your own projects as is, copy the files together with the `tracker_wrapper.h` script from the `usc_code` folder that it relies on.