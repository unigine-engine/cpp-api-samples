// Demonstrates passing UNIGINE objects (Image) between C++ and UnigineScript. Three
// approaches are shown: direct ImagePtr, Variable with setImage/getImage, and the
// TypeToVariable/VariableToType template system for automatic type conversion.

#include "ScriptTransfer.h"

#include "../../utils/UnigineScriptsInterpreter.h"

#include <UnigineInterface.h>
#include <UnigineWorld.h>
#include <UnigineConsole.h>

REGISTER_COMPONENT(ScriptTransfer)

using namespace Unigine;


// ============================================================================
// Receiving Objects - three ways to accept UNIGINE objects from script
// ============================================================================

// Method 0: Direct ImagePtr parameter (automatic conversion)
void my_image_info_0(ImagePtr image)
{
	Log::message("%s my_image_info_0(): %s %d %d (%p)\n", sourse_str, image->getFormatName(),
		image->getWidth(), image->getHeight(), image.get());
}

// Method 1: Variable parameter with explicit getImage extraction
void my_image_info_1(const Variable &v)
{
	ImagePtr image = v.getImage(Interpreter::get());

	Log::message("%s my_image_info_1(): %s %d %d (%p)\n", sourse_str, image->getFormatName(),
		image->getWidth(), image->getHeight(), image.get());
}

// Method 2: Variable parameter with VariableToType template conversion
void my_image_info_2(const Variable &v)
{
	ImagePtr image = VariableToType<ImagePtr>(Interpreter::get(), v).value;

	Log::message("%s my_image_info_2(): %s %d %d (%p)\n", sourse_str, image->getFormatName(),
		image->getWidth(), image->getHeight(), image.get());
}

// ============================================================================
// Returning Objects - three ways to return UNIGINE objects to script
// ============================================================================

// Persistent references to keep images alive after function returns
ImagePtr image_0;
ImagePtr image_1;
ImagePtr image_2;

// Method 0: Return ImagePtr directly (automatic conversion)
ImagePtr my_image_create_0()
{
	image_0 = Image::create();
	image_0->create2D(128, 128, Image::FORMAT_RG8);

	return image_0;
}

// Method 1: Return Variable with explicit setImage wrapping
Variable my_image_create_1()
{
	image_1 = Image::create();
	image_1->create2D(128, 128, Image::FORMAT_RG8);

	Variable v;
	v.setImage(Interpreter::get(), image_1);
	return v;
}

// Method 2: Return Variable via TypeToVariable template conversion
Variable my_image_create_2()
{
	image_2 = Image::create();
	image_2->create2D(128, 128, Image::FORMAT_RG8);

	return TypeToVariable<ImagePtr>(Interpreter::get(), image_2).value;
}

// ============================================================================
// Function Registration
// ============================================================================

// Deferred registration: exports all image info and create functions
USCInterpreter transfer_interpreter([]() {
	Interpreter::addExternFunction("my_image_info_0", MakeExternFunction(&my_image_info_0));
	Interpreter::addExternFunction("my_image_info_1", MakeExternFunction(&my_image_info_1));
	Interpreter::addExternFunction("my_image_info_2", MakeExternFunction(&my_image_info_2));
	Interpreter::addExternFunction("my_image_create_0", MakeExternFunction(&my_image_create_0));
	Interpreter::addExternFunction("my_image_create_1", MakeExternFunction(&my_image_create_1));
	Interpreter::addExternFunction("my_image_create_2", MakeExternFunction(&my_image_create_2));
});


// Configures on-screen console for displaying transfer results
void ScriptTransfer::init()
{
	Console::setOnscreen(true);
	Console::setOnscreenFontSize(15);
	Console::setOnscreenTime(1000);
	Console::setOnscreenHeight(100);
}

// Restores default console settings
void ScriptTransfer::shutdown()
{
	Console::setOnscreen(false);
	Console::setOnscreenHeight(30);
}
