# Node State Save-Restore

This sample demonstrates how to save and restore the state of an arbitrary Node using the *saveState()* and *restoreState()* methods. In this sample we:

-At *Initialization* create a ***Blob*** container for state storage and save the initial state of the referenced node.
-Enable visualizer (a semi-transparent box) at saved position for visualization.
-Save and restore the Node's state at any time by clicking the sample UI panel's *Save* and *Restore* buttons.

**Use Cases**:

-Game saves
-Undo/Redo systems
-Scene serialization