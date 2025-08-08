# Day-Night Switching

This sample showcases a dynamic day-night cycle driven by the rotation of a **World Light** source (sun), animated according to a simulated global time. The sun's position is updated continuously or in response to manual time input. The time progression speed can be adjusted using the *Timescale* slider.

The sun's orientation influences both the overall scene lighting and object-specific responses, enabling or disabling nodes and adjusting the emission states of designated materials depending on whether it's currently day or night. Additional props, such as **Projected Light** node and door closed/open signage, are toggled to reflect the time of day. Red and blue helper vectors visualize the zenith direction and the sun's current orientation, respectively.

**Two control modes are available:**

+**Zenith Angle**: Uses the angle between the sun's direction and the zenith (up vector). If the angle is below a threshold, it is considered daytime.
+**Time-Based**: Defines day/night using configurable *Morning* and *Evening hour boundaries* sliders.

**Use Cases:**

Ideal for **games, simulations**, or **architectural walkthroughs** that need consistent lighting transitions and dynamic environmental response.