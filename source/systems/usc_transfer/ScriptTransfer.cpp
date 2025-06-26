#include "ScriptTransfer.h"

#include "../../utils/UnigineScriptsInterpreter.h"

#include <UnigineInterface.h>
#include <UnigineWorld.h>
#include <UnigineConsole.h>

REGISTER_COMPONENT(ScriptTransfer)

using namespace Unigine;


//////////////////////////////////////////////////////////////////////////
// Info function
//////////////////////////////////////////////////////////////////////////

void my_image_info_0(ImagePtr image)
{
	Log::message("%s my_image_info_0(): %s %d %d (%p)\n", sourse_str, image->getFormatName(),
		image->getWidth(), image->getHeight(), image.get());
}

void my_image_info_1(const Variable &v)
{
	ImagePtr image = v.getImage(Interpreter::get());

	Log::message("%s my_image_info_1(): %s %d %d (%p)\n", sourse_str, image->getFormatName(),
		image->getWidth(), image->getHeight(), image.get());
}

void my_image_info_2(const Variable &v)
{
	ImagePtr image = VariableToType<ImagePtr>(Interpreter::get(), v).value;

	Log::message("%s my_image_info_2(): %s %d %d (%p)\n", sourse_str, image->getFormatName(),
		image->getWidth(), image->getHeight(), image.get());
}

//////////////////////////////////////////////////////////////////////////
// Create function
//////////////////////////////////////////////////////////////////////////

ImagePtr image_0;
ImagePtr image_1;
ImagePtr image_2;

ImagePtr my_image_create_0()
{
	image_0 = Image::create();
	image_0->create2D(128, 128, Image::FORMAT_RG8);

	return image_0;
}

Variable my_image_create_1()
{
	image_1 = Image::create();
	image_1->create2D(128, 128, Image::FORMAT_RG8);

	Variable v;
	v.setImage(Interpreter::get(), image_1);
	return v;
}

Variable my_image_create_2()
{
	image_2 = Image::create();
	image_2->create2D(128, 128, Image::FORMAT_RG8);

	return TypeToVariable<ImagePtr>(Interpreter::get(), image_2).value;
}


USCInterpreter transfer_interpreter([]() {
	// export functions
	Interpreter::addExternFunction("my_image_info_0", MakeExternFunction(&my_image_info_0));
	Interpreter::addExternFunction("my_image_info_1", MakeExternFunction(&my_image_info_1));
	Interpreter::addExternFunction("my_image_info_2", MakeExternFunction(&my_image_info_2));
	Interpreter::addExternFunction("my_image_create_0", MakeExternFunction(&my_image_create_0));
	Interpreter::addExternFunction("my_image_create_1", MakeExternFunction(&my_image_create_1));
	Interpreter::addExternFunction("my_image_create_2", MakeExternFunction(&my_image_create_2));
});


void ScriptTransfer::init()
{
	Console::setOnscreen(true);
	Console::setOnscreenFontSize(15);
	Console::setOnscreenTime(1000);
	Console::setOnscreenHeight(100);
}

void ScriptTransfer::shutdown()
{
	Console::setOnscreen(false);
	Console::setOnscreenHeight(30);
}
