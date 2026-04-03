// Lists all registered properties and materials in the project. Demonstrates
// Properties:: and Materials:: static access for iterating over the asset database.
// Useful for debugging material/property inheritance hierarchies.

#include "MaterialsAndPropertiesSample.h"

#include <UnigineConsole.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(MaterialsAndPropertiesSample)

// Console is enabled and all properties/materials are listed.
void MaterialsAndPropertiesSample::sample()
{
	Console::setOnscreen(true);
	Console::setOnscreenHeight(100);

	// Properties
	{
		Log::message("Properties:\n");

		// Loop through all available properties
		for (int j = 0; j < Properties::getNumProperties(); j++)
		{
			PropertyPtr property = Properties::getProperty(j);

			// Print property name and number of child properties
			Log::message("Property: %s (%d children)\n", 
				property->getName(),
				property->getNumChildren());
		}
	}

	Log::message("\n");

	// Materials
	{
		// Print number of materials
		Log::message("Materials: %d\n", Materials::getNumMaterials());

		// Loop through all materials
		for (int i = 0; i < Materials::getNumMaterials(); i++)
		{
			MaterialPtr material = Materials::getMaterial(i);

			// Print material filepath and number of child materials
			Log::message("\t%asset: materials: %d\n", 
				material->getFilePath().get(),
				material->getNumChildren());
		}
	}
}


// Console is hidden and height is reset.
void MaterialsAndPropertiesSample::shutdown()
{
	Console::setOnscreen(false);
	Console::setOnscreenHeight(30);
}
