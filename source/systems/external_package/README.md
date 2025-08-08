# External Package

This sample demonstrates how to create a custom data package using code and use it to generate objects in the scene.

Package is a collection of files and data for UNIGINE projects stored a single file. The *Package* class is a data provider for the File System. You can use it to load all necessary resources. 

The *ExternalPackage* class describes the generation of a mesh (in this case, a box) and its saving to a temporary file at a specified path. The class also implements an interface for searching, reading, and retrieving information about files within the package.

The *ExternalPackageSample* class adds the created external package, and its contents are used to create meshes with different positions and orientations. This approach allows for quick and convenient management of a large number of objects without adding them by hand.

Packages can be used to conveniently transfer files between your projects or exchange data with other users, be it content (a single model or a scene with a set of objects driven by logic implemented via C# components) or files (plugins, libraries, execution files, etc.).

Using this example will help you understand how to organize work with external files, create and manage your own data packages, implement a mechanism for loading and reading data from a package.