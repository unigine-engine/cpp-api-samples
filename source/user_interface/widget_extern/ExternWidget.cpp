#include "ExternWidget.h"

REGISTER_COMPONENT(ExternWidget);

using namespace Unigine;
using namespace Math;

MyWidget::MyWidget(GuiPtr gui, const char *str)
	: WidgetExternBase(gui)
{
	setTime(0.0f);
	setText(str);
	setColor(vec4(1.0f));
}

MyWidget::MyWidget(void *widget)
	: WidgetExternBase(widget)
{
	setTime(0.0f);
	setColor(vec4(1.0f));
}

MyWidget::~MyWidget()
{}

int MyWidget::getClassID()
{
	return 1;
}


void MyWidget::update(float ifps)
{
	time += ifps;
	WidgetExternBase::update(ifps);
}

void MyWidget::checkCallbacks(int x, int y)
{
	check_default_callbacks(x, y);
	check_pressed_callbacks(x, y);
}

void MyWidget::arrange()
{
	int width = 0;
	int height = 0;
	get_text_unit_size(text.get(), width, height);
	set_width(max(get_default_width(), width));
	set_height(max(get_default_height(), height));
}

void MyWidget::expand(int width, int height)
{
	set_width(max(width, get_width()));
}

void MyWidget::render()
{
	push_transform();
	set_translate(get_position_x(), get_position_y());

	float offset = get_width() / 2.0f;
	set_transform(translate(vec3(offset, 0.0f, -10.0f)) * rotateY((float)Unigine::Math::sin(time * 2.0f) * 45.0f) * translate(vec3(-offset, 0.0f, 10.0f)));

	render_text(0, 0, color, text.get(), 0, 0);

	pop_transform();
}

void MyWidget::setTime(float t)
{
	time = t;
}

void MyWidget::setText(const char *str)
{
	text = str;
}

void MyWidget::setColor(const vec4 &c)
{
	color = c;
}

void ExternWidget::init()
{
	// register MyWidget class constructor
	WidgetExternBase::addClassID<MyWidget>(1);
	
	EngineWindowViewportPtr main_window = WindowManager::getMainWindow();

	GuiPtr gui = main_window->getGui();

	window = WidgetWindow::create(gui, "WidgetWindow", 4, 4);

	my_widget_0 = new MyWidget(gui, "My Widget 0");
	window->addChild(my_widget_0->getWidget(), Gui::ALIGN_EXPAND);
	WidgetPtr widget_0 = my_widget_0->getWidget();
	widget_0->setFontSize(48);
	
	WidgetExternPtr widget_1 = WidgetExtern::create(gui, 1);
	my_widget_1 = static_cast<MyWidget *>(widget_1->getWidgetExtern());
	my_widget_1->setTime(2.0f);
	my_widget_1->setText("My Widget 1");
	my_widget_1->setColor(vec4(1.0f, 1.0f, 0.0f, 1.0f));
	widget_1->setFontSize(48);
	window->addChild(widget_1, Gui::ALIGN_EXPAND);

	window->arrange();
	
	main_window->addChild(window, Gui::ALIGN_OVERLAP | Gui::ALIGN_CENTER);
}

void ExternWidget::shutdown()
{
	if (my_widget_0)
		delete my_widget_0;

	if (my_widget_1)
		delete my_widget_1;

	window.deleteLater();
}
