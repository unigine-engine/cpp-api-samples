# FMOD Core

Before running this sample, follow the steps provided in the setup section or in the FMOD Integration Guide linked below.

This sample loads the *FMOD* plugin and initializes the *Core* system to demonstrate playback of both 2D and 3D sound instances, supporting up to **1024** simultaneous sound channels. You can switch between the 2D and 3D scenes to observe and interact with different sound behaviors.

Two sound modes are presented:

+**2D Music Playback**: Standard audio playback with support for volume adjustment and a distortion DSP effect
+**3D Music Playback**: Sound is emitted from a red sphere in 3D space, demonstrating positional audio in the environment. Move the camera to experience changes in sound direction and attenuation in real time.

**Use Cases:**

+Integrating FMOD Core into runtime environments for games or simulations
+Prototyping spatial sound design and 3D audio placement.

**Sample Requirements**

1. Download and install **FMOD Engine (version 2.02.04)** for your OS available on the official website (**![https://www.fmod.com/download](https://www.fmod.com/download)**).

2. Go to the FMOD installation folder and copy the following files to the `bin` folder of your project:

**for Windows:**

+`fmod.dll, fmodL.dll` from `/api/core/lib/x64/`
+`fsbank.dll, libfsbvorbis64.dll, opus.dll` from `/api/fbank/lib/x64/`.
+`fmodstudio.dll, fmodstudioL.dll` from `/api/studio/lib/x64/`

**for Linux**

+`libfmod.so.13, libfmodL.so.13` from `/api/core/lib/x64/`
+`libfsbank.so.13, libfsbankL.so.13, libfsbvorbis.so, libopus.so` from `/api/fbank/lib/x64/`
+`libfmodstudio.so.13, libfmodstudioL.so.13` from `/api/studio/lib/x64/`