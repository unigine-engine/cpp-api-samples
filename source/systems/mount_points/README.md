# Mount Points

This sample demonstrates the functionality of mount points in the Engine file system.
*MountPointsSample.cpp* allows you to add or remove mount points for a folder and a package archive via API. If the mount point is active, an image stored inside will be loaded and displayed.

Mounted paths are shown in the *UI* window, where you can toggle between mounting or unmounting each resource. Images are accessed using virtual paths defined by the mount location.

The sample illustrates the concept of virtualized file access: if a resource is not available via a mount point, it will not be found or displayed by the Engine.

This approach is useful for working with external content (stored outside the **data** folder), modular data loading, or switching asset sets at runtime.