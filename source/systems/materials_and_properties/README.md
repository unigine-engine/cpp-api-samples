# Materials and Properties

This sample demonstrates how to access all materials and properties in the project via API.
The sample iterates through the list of registered in the *Property Manager* via *Properties::getProperty()* and prints out the names and child counts for each. It also gets all available materials from the *Materials Manager* via *Materials::getMaterial()*, and lists them along with their file paths and number of children.

This can be used as a reference for accessing and working with project assets at runtime - whether for inspection, dynamic assignment, or content management logic.