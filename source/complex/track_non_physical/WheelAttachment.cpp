#include "WheelAttachment.h"

REGISTER_COMPONENT(WheelAttachment);

using namespace Unigine;
using namespace Math;

void WheelAttachment::init()
{
	wheel_parent = attached_to_wheel->getParent();
	wheel_local_transform = wheel_parent->getIWorldTransform() * attached_to_wheel->getWorldTransform();
	track_local_transform = attached_to_wheel->getIWorldTransform() * node->getWorldTransform();
}

void WheelAttachment::update()
{
	// change position only, ignore rotation of the wheel
	wheel_local_transform.setColumn3(3, attached_to_wheel->getPosition());
	node->setWorldTransform(wheel_parent->getWorldTransform() * wheel_local_transform * track_local_transform);
}
