# Physics-Based Animation

This sample demonstrates several ways of moving an object towards a target smoothly, shown as a simple game.

The sample has two modes, switched with the buttons in the description window.

In the **Animations Demo** mode, the way of moving is selected from the drop-down list, with each of them implemented as a separate component:

-**Linear** - moves at a constant speed.
-**EaseIn** - accelerates gradually.
-**EaseInOut** - accelerates at the start and decelerates at the end.
-**EaseOut** - decelerates gradually.
-**EaseOutElastic** - oscillates around the destination before settling on it.
-**EaseOutBack** - overshoots the destination slightly and comes back to it.
-**EaseOutBounce** - bounces at the destination.

In the **Start Game** mode, you control the laser pointer with the mouse, and the cat chases it using spring simulation, becoming faster and more responsive over time until it catches the pointer.

The parameters of each way of moving are available in the properties of the corresponding component in UnigineEditor.

Easing and spring-based motion make the movement of objects look natural, which is useful for cameras, UI elements, followers, and any object that has to reach a moving target smoothly.