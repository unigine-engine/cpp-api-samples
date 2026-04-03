// Demonstrates creating custom widgets by extending WidgetExternBase. MyWidget renders
// text with a 3D rotation effect applied via setTransform. Two instantiation methods
// are shown: direct construction and factory via WidgetExtern class ID registration.

#include "ExternWidget.h"

REGISTER_COMPONENT(ExternWidget);

using namespace Unigine;
using namespace Math;

// Widget is initialized with GUI context, text content, and default white color.
MyWidget::MyWidget(GuiPtr gui, const char *str)
	: WidgetExternBase(gui)
{
	setTime(0.0f);
	setText(str);
	setColor(vec4(1.0f));
}

// Factory constructor wraps existing widget; text is set separately via setText.
MyWidget::MyWidget(void *widget)
	: WidgetExternBase(widget)
{
	setTime(0.0f);
	setColor(vec4(1.0f));
}

MyWidget::~MyWidget()
{}

// Class ID 1 is used for factory registration with addClassID<MyWidget>(1).
int MyWidget::getClassID()
{
	return 1;
}


// Animation time is accumulated each frame for rotation calculation.
void MyWidget::update(float ifps)
{
	time += ifps;
	WidgetExternBase::update(ifps);
}

// Both default and pressed callback types are processed for input handling.
void MyWidget::checkCallbacks(int x, int y)
{
	check_default_callbacks(x, y);
	check_pressed_callbacks(x, y);
}

// Widget size is calculated from text dimensions, respecting minimum defaults.
void MyWidget::arrange()
{
	int width = 0;
	int height = 0;
	// Measure text string to determine required size
	get_text_unit_size(text.get(), width, height);
	set_width(max(get_default_width(), width));
	set_height(max(get_default_height(), height));
}

// Width is expanded to fit container; height remains unchanged.
void MyWidget::expand(int width, int height)
{
	set_width(max(width, get_width()));
}

// Text is rendered with oscillating Y-axis rotation around widget center.
void MyWidget::render()
{
	push_transform();
	set_translate(get_position_x(), get_position_y());

	// Rotation pivot is placed at horizontal center of widget
	float offset = get_width() / 2.0f;
	// Transform: translate to pivot, rotate based on time, translate back
	set_transform(translate(vec3(offset, 0.0f, -10.0f)) * rotateY((float)Unigine::Math::sin(time * 2.0f) * 45.0f) * translate(vec3(-offset, 0.0f, 10.0f)));

	render_text(0, 0, color, text.get(), 0, 0);

	pop_transform();
}

// Sets animation phase offset for staggered animation between instances.
void MyWidget::setTime(float t)
{
	time = t;
}

// Updates displayed text content.
void MyWidget::setText(const char *str)
{
	text = str;
}

// Sets RGBA color for text rendering.
void MyWidget::setColor(const vec4 &c)
{
	color = c;
}

// Two custom widgets are created: one via direct construction, one via factory.
void ExternWidget::init()
{
	// Register MyWidget class constructor for factory instantiation
	WidgetExternBase::addClassID<MyWidget>(1);

	EngineWindowViewportPtr main_window = WindowManager::getMainWindow();

	GuiPtr gui = main_window->getGui();

	window = WidgetWindow::create(gui, "WidgetWindow", 4, 4);

	// Method 1: Direct construction with text parameter
	my_widget_0 = new MyWidget(gui, "My Widget 0");
	window->addChild(my_widget_0->getWidget(), Gui::ALIGN_EXPAND);
	WidgetPtr widget_0 = my_widget_0->getWidget();
	widget_0->setFontSize(48);

	// Method 2: Factory construction via WidgetExtern using class ID
	WidgetExternPtr widget_1 = WidgetExtern::create(gui, 1);
	my_widget_1 = static_cast<MyWidget *>(widget_1->getWidgetExtern());
	// Animation phase offset creates visual difference between instances
	my_widget_1->setTime(2.0f);
	my_widget_1->setText("My Widget 1");
	// Yellow color distinguishes factory-created instance
	my_widget_1->setColor(vec4(1.0f, 1.0f, 0.0f, 1.0f));
	widget_1->setFontSize(48);
	window->addChild(widget_1, Gui::ALIGN_EXPAND);

	window->arrange();

	main_window->addChild(window, Gui::ALIGN_OVERLAP | Gui::ALIGN_CENTER);
}

// Custom widgets are manually deleted; window uses smart pointer cleanup.
void ExternWidget::shutdown()
{
	if (my_widget_0)
		delete my_widget_0;

	if (my_widget_1)
		delete my_widget_1;

	window.deleteLater();
}
