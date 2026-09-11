# Material Parameters Animation

This sample illustrates how to change the parameters of *materials* at runtime. Each object in the scene has its own component that animates one of the parameters continuously:

-**Albedo color** - smoothly transitions between two colors.
-**Albedo texture** - alternates between two textures loaded from files.
-**Metalness** - smoothly transitions between two values.
-**Emission** state - is switched on and off, making the object blink.
-**Cast World Shadow** state - is switched on and off, making the shadow of the object appear and disappear.

The parameters can be edited in the properties of the corresponding components in UnigineEditor. The current value of every animated parameter is displayed in the *State* window.

Changing material parameters at runtime is the basis for various dynamic effects, such as highlighting the objects the user interacts with, indicating the state of a device, or reproducing the changes of a surface over time.