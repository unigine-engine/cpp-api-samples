# JSON

This sample shows how to generate a structured *JSON* document containing objects, arrays, and various data types such as strings, numbers, booleans, and null values. It also demonstrates how to traverse and print this structure recursively with indentation, imitating a pretty-printed output.

The sample begins by constructing a custom *JSON* structure in memory using *Json::create()* and its child manipulation methods. Nodes are added dynamically and include both named and unnamed children of various types. Once built, the structure is traversed recursively and printed to the Console in a readable format using indentation and commas, based on node type and position. The code demonstrates how to distinguish between arrays, objects, and primitive values when printing.

This sample is useful for learning the basics of *JSON* manipulation, such as creating structured data, traversing an element tree, and formatting output. It can serve as a foundation for processing *JSON* data (e.g., responses to *REST API* requests), as well as for complex serialization or debugging tools.