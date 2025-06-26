#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

class Callbacks final : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(Callbacks, ComponentBase)

	COMPONENT_INIT(init)
	COMPONENT_SHUTDOWN(shutdown)


private:
	void init();
	void shutdown();

	void method_0();
	void method_1(int a);
	void method_2(int a, int b);
	void method_3(int a, int b, int c);
	void method_4(int a, int b, int c, int d);
};
