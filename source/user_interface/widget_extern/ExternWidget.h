// Demonstrates custom widget creation by extending WidgetExternBase. MyWidget
// renders animated text with a 3D rotation transform. Two instantiation methods
// are shown: direct construction and factory via class ID registration.

#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

// Custom widget that renders text with animated 3D rotation effect.
class MyWidget : public Unigine::WidgetExternBase
{
private:
	// Elapsed time for animation phase calculation
	float time;
	// Text content displayed by the widget
	Unigine::String text;
	// RGBA color applied to rendered text
	Unigine::Math::vec4 color;

public:
	// Constructs widget with GUI context and initial text
	MyWidget(Unigine::GuiPtr gui, const char *str);
	// Constructs from existing widget pointer (used by factory)
	MyWidget(void *widget);
	~MyWidget() override;

	// Returns unique class ID for factory registration
	int getClassID() override;

	// Advances animation time each frame
	void update(float ifps) override;

	// Processes default and pressed callback events
	void checkCallbacks(int x, int y) override;

	// Calculates widget size based on text dimensions
	void arrange() override;
	// Expands width to fit parent container
	void expand(int width, int height) override;
	// Draws text with animated 3D rotation transform
	void render() override;

	// Sets animation time offset
	void setTime(float t);
	// Sets text content to display
	void setText(const char *str);
	// Sets text rendering color
	void setColor(const Unigine::Math::vec4 &c);
};

// Component that creates and manages custom MyWidget instances in a window.
class ExternWidget : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ExternWidget, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();

	// Widget created via direct constructor
	MyWidget *my_widget_0 = nullptr;
	// Widget created via WidgetExtern factory
	MyWidget *my_widget_1 = nullptr;
	// Container window for custom widgets
	Unigine::WidgetWindowPtr window;
};