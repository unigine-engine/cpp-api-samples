#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

// Demonstrates the Unigine callback system for deferred function invocation.
// MakeCallback() wraps free functions and member methods into CallbackBase objects
// that can be executed later with default or runtime-specified arguments.
// Supports 0-4 argument functions with argument binding at creation time.
class Callbacks final : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(Callbacks, ComponentBase)
	COMPONENT_DESCRIPTION("This component demonstrates the Unigine callback system by wrapping free functions "
			"and member methods into Callback objects and executing them with different arguments.")

	COMPONENT_INIT(init)
	COMPONENT_SHUTDOWN(shutdown)


private:
	void init();
	void shutdown();

	// Example member methods to be called via callbacks
	void method_0();
	void method_1(int a);
	void method_2(int a, int b);
	void method_3(int a, int b, int c);
	void method_4(int a, int b, int c, int d);
};
