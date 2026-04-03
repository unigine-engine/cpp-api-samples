# FMOD Studio

Before running this sample, follow the steps provided in the setup section or in the FMOD Integration Guide linked below.

This sample demonstrates how to control, simulate, and visualize real-time audio events using the *FMOD Studio* plugin.

The setup includes a stationary object (car) and a moving object representing a source of Doppler-shifted sound, which can be toggled and adjusted via the *Doppler* tab of the GUI Parameters section. Additional sound settings such as wind, rain, the car engine RPM sound, and overall environmental volume can also be fine-tuned in real time under the *Ambience, Engine*, and *VCA* tabs.

**Use Cases:**

-Prototyping audio-driven game mechanics (e.g., vehicle audio, spatial ambiences, Doppler effects)
-Demonstrating dynamic audio parameters and movement-based effects like Doppler shifts.

**Sample Requirements**

1. Download and install **FMOD Engine (version 2.03.08)** for your OS available on the official website (**![https://www.fmod.com/download](https://www.fmod.com/download)**).

2. Go to the FMOD installation folder and copy the following files to the `bin` folder of your project:

**for Windows:**

-`fmod.dll, fmodL.dll` from `/api/core/lib/x64/`
-`fsbank.dll, libfsbvorbis64.dll, opus.dll` from `/api/fbank/lib/x64/`.
-`fmodstudio.dll, fmodstudioL.dll` from `/api/studio/lib/x64/`

**for Linux**

-`libfmod.so.13, libfmodL.so.13` from `/api/core/lib/x64/`
-`libfsbank.so.13, libfsbankL.so.13, libfsbvorbis.so, libopus.so` from `/api/fbank/lib/x64/`
-`libfmodstudio.so.13, libfmodstudioL.so.13` from `/api/studio/lib/x64/`