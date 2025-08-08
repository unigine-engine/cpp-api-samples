# USC Structures

This sample demonstrates how to expose *C++* structs to *UnigineScript* using the *Interpreter* class.
It defines a simple *C++* structure *MyVector* with four float fields (*x, y, z, w*) and registers it. Each field is mapped via explicit getter and setter methods to allow full read/write access from the *UnigineScript* side.