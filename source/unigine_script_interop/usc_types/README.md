# Usc Types

This sample demonstrates how to enable type conversion between custom *C++* types and *UnigineScript* using the Variable class.

A user-defined *MyVector3* class is introduced to represent a *3D* vector. To integrate this class with the *UnigineScript* environment, custom specializations of the *TypeToVariable* and *VariableToType* templates are implemented. These allow automatic conversion between *MyVector3* and *vec3* values inside the *UnigineScript* runtime.