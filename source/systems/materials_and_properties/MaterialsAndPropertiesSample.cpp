#include "MaterialsAndPropertiesSample.h"

#include <UnigineConsole.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(MaterialsAndPropertiesSample)

void MaterialsAndPropertiesSample::sample()
{
	Console::setOnscreen(true);
	Console::setOnscreenHeight(100);

	// properties
	{
		// property manager info
		Log::message("Properties:\n");
		for (int j = 0; j < Properties::getNumProperties(); j++)
		{
			PropertyPtr property = Properties::getProperty(j);

			// property info
			Log::message("Property: %s (%d children)\n", property->getName(),
				property->getNumChildren());
		}
	}

	Log::message("\n");

	// materials
	{
		// material library info
		Log::message("Materials: %d\n", Materials::getNumMaterials());

		for (int i = 0; i < Materials::getNumMaterials(); i++)
		{
			MaterialPtr material = Materials::getMaterial(i);

			// material info
			Log::message("\t%asset: materials: %d\n", material->getFilePath().get(),
				material->getNumChildren());
		}
	}
}


void MaterialsAndPropertiesSample::shutdown()
{
	Console::setOnscreen(false);
	Console::setOnscreenHeight(30);
}
