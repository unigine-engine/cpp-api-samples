# Ambient Sound

This sample shows the implementation of a component that controls the playback of ambient sound (*AmbientSource*) with interactive GUI controls using the *AmbientSource* class. The component creates an ambient sound source that plays continuously in the background and provides real-time parameter adjustment through a dedicated interface.

The streaming mode can be toggled dynamically via the *Stream* checkbox, recreating the sound source to switch between preloaded audio (lower latency) and streamed audio (lower memory usage). This approach is ideal for background music, environmental sounds, and other continuous audio elements that enhance scene immersion.