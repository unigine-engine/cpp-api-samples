#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

class MyWidget : public Unigine::WidgetExternBase
{
private:
	float time;
	Unigine::String text;
	Unigine::Math::vec4 color;

public:
	MyWidget(Unigine::GuiPtr gui, const char *str);
	MyWidget(void *widget);
	~MyWidget() override;

	// unique class ID
	int getClassID() override;

	// update widget
	void update(float ifps) override;

	// check events
	void checkCallbacks(int x, int y) override;

	// render widget
	void arrange() override;
	void expand(int width, int height) override;
	void render() override;

	// parameters
	void setTime(float t);
	void setText(const char *str);
	void setColor(const Unigine::Math::vec4 &c);
};

class ExternWidget : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ExternWidget, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();

	MyWidget *my_widget_0 = nullptr;
	MyWidget *my_widget_1 = nullptr;
	Unigine::WidgetWindowPtr window;
};