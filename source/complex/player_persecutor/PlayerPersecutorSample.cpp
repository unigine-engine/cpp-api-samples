#include "PlayerPersecutorSample.h"
#include "PlayerPersecutorComponent.h"


REGISTER_COMPONENT(PlayerPersecutorSample);

using namespace Unigine;

void PlayerPersecutorSample::init()
{
	mouse_grab_state_at_init = Input::isMouseGrab();
	mouse_handle_at_init = Input::getMouseHandle();
	Input::setMouseGrab(false);
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);

	description_window.createWindow();

	WidgetGroupBoxPtr group = WidgetGroupBox::create();

	WidgetCheckBoxPtr fixed = WidgetCheckBox::create();
	fixed->setLifetime(Widget::LIFETIME_WORLD);
	fixed->setText(" fixed");
	fixed->getEventChanged().connect(*this, [this, fixed] () {
		PlayerPersecutorComponent* component = getComponent<PlayerPersecutorComponent>(persecutor);
		if (component)
		{
			component->setFixed(fixed->isChecked());
		}
		});

	WidgetCheckBoxPtr controlled = WidgetCheckBox::create();
	controlled->setLifetime(Widget::LIFETIME_WORLD);
	controlled->setText(" controlled");
	controlled->setChecked(true);
	controlled->getEventChanged().connect(*this, [this, controlled]() {
		PlayerPersecutorComponent* component = getComponent<PlayerPersecutorComponent>(persecutor);
		if (component)
		{
			component->controlled = controlled->isChecked();
		}
		});

	WidgetCheckBoxPtr collision = WidgetCheckBox::create();
	collision->setLifetime(Widget::LIFETIME_WORLD);
	collision->setText(" collision");
	collision->setChecked(true);
	collision->getEventChanged().connect(*this, [this, collision]() {
		PlayerPersecutorComponent* component = getComponent<PlayerPersecutorComponent>(persecutor);
		if (component)
		{
			component->setCollision(collision->isChecked());
		}
		});

	group->addChild(fixed, Gui::ALIGN_LEFT);
	group->addChild(controlled, Gui::ALIGN_LEFT);
	group->addChild(collision, Gui::ALIGN_LEFT);

	description_window.getWindow()->addChild(group);



	description_window.addFloatParameter("Min Distance", "", 1.0f, 0.0f, 50.0f, [this](float value) {
		PlayerPersecutorComponent* component = getComponent<PlayerPersecutorComponent>(persecutor);
		if (component)
		{
			component->setMinDistance(value);
		}
		});

	description_window.addFloatParameter("Max Distance", "", 4.0f, 0.0f, 50.0f, [this](float value) {
		PlayerPersecutorComponent* component = getComponent<PlayerPersecutorComponent>(persecutor);
		if (component)
		{
			component->setMaxDistance(value);
		}
		});

	description_window.addFloatParameter("Min ThetaAngle", "", -89.0f, -89.0f, 89.0f, [this](float value) {
		PlayerPersecutorComponent* component = getComponent<PlayerPersecutorComponent>(persecutor);
		if (component)
		{
			component->setMinThetaAngle(value);
		}
		});

	description_window.addFloatParameter("Max ThetaAngle", "", 89.0f, -89.0f, 89.0f, [this](float value) {
		PlayerPersecutorComponent* component = getComponent<PlayerPersecutorComponent>(persecutor);
		if (component)
		{
			component->setMaxThetaAngle(value);
		}
		});

	description_window.addParameterSpacer();


	description_window.addFloatParameter("Anchor Point X", "", 0.0f, -20.0f, 20.0f, [this](float value) {
		PlayerPersecutorComponent* component = getComponent<PlayerPersecutorComponent>(persecutor);
		if (component)
		{
			component->setAnchor(Unigine::Math::vec3(value, component->getAnchor().y, component->getAnchor().z));
		}
		});

	description_window.addFloatParameter("Anchor Point Y", "", 0.0f, -20.0f, 20.0f, [this](float value) {
		PlayerPersecutorComponent* component = getComponent<PlayerPersecutorComponent>(persecutor);
		if (component)
		{
			component->setAnchor(Unigine::Math::vec3(component->getAnchor().x, value, component->getAnchor().z));
		}
		});

	description_window.addFloatParameter("Anchor Point Z", "", 0.0f, -20.0f, 20.0f, [this](float value) {
		PlayerPersecutorComponent* component = getComponent<PlayerPersecutorComponent>(persecutor);
		if (component)
		{
			component->setAnchor(Unigine::Math::vec3(component->getAnchor().x, component->getAnchor().y, value));
		}
		});

}

void PlayerPersecutorSample::shutdown()
{
	Input::setMouseGrab(mouse_grab_state_at_init);
	Input::setMouseHandle(mouse_handle_at_init);

	description_window.shutdown();
}
