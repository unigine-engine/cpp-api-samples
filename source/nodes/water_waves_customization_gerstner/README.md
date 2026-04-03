# Water Waves Customization Gerstner

This sample demonstrates how to control the wave spectrum of **Global Water** in **Manual** mode via API over wave parameters (octaves - a set of waves at a specific size/frequency level, count per octave, length, amplitude, phase, and steepness).

Because all wave parameters are managed on the CPU side, they can be dynamically generated, modified, or stored for reproducible results and external integration (for example, to process Weather Control packets from IOS).

> [!NOTE]The maximum number of waves is **256**, but higher values significantly affect performance, so we recommend keeping the number below 100 and using the minimum possible number.