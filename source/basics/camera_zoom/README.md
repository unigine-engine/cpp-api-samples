# Camera Zoom

This sample demonstrates a zoom and camera focus system that allows the user to inspect predefined targets in the scene and adjust the zoom level. Three info boards are placed throughout the scene, each with an in-world GUI panel showing its distance from the player and its dimensions. These panels update automatically in real time based on the player's position.

The user can select any target using dedicated UI buttons, prompting the camera to focus on the selected object.

Zoom is controlled via a slider that adjusts the camera&#8217;s field of view. As the FOV changes, related parameters such as mouse sensitivity and render distance scaling are adjusted as well to maintain a consistent experience. A reset button restores all values to default.

This setup is useful for scenarios that require object-focused viewing, such as inspection tools, scene walkthroughs, or any case where adjustable zoom and camera focus help users better understand or explore the scene.