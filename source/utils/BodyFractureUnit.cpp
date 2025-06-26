#include "BodyFractureUnit.h"
#include <UnigineVisualizer.h>

REGISTER_COMPONENT(BodyFractureUnit);

using namespace Unigine;

BodyFracturePtr BodyFractureUnit::getBody() const
{
	return own_body;
}

bool BodyFractureUnit::isBroken()
{
	return own_body ? own_body->isBroken() : false;
}

void BodyFractureUnit::crack(float impulse, const Unigine::Math::Vec3 &point, const Unigine::Math::vec3 &normal)
{
	if (own_body && !own_body->isBroken() && impulse >= max_impulse)
		crack(own_body, point, normal);
}

void BodyFractureUnit::init()
{
	own_body = dynamic_ptr_cast<BodyFracture>(node->getObjectBody());
	if (!own_body)
	{
		Log::warning("BodyFractureUnit::init(): no fracture body was found\n");
		return;
	}

	own_body->setThreshold(threshold);
	own_body->setMaterial(material);

	own_body->getEventContactEnter().connect(this, &BodyFractureUnit::onContactEnter);

	visualizer_enabled = Visualizer::isEnabled();
	Visualizer::setEnabled(true);
}

void BodyFractureUnit::update()
{
	if (!debug)
		return;

	if (!own_body || !own_body->isBroken())
	{
		Visualizer::renderObject(checked_ptr_cast<Object>(node), Math::vec4_white);
		return;
	}

	int cnt = own_body->getNumChildren();
	for (int i = 0; i < cnt; ++i)
		Visualizer::renderObject(own_body->getChild(i)->getObject(), Math::vec4_green);
}

void BodyFractureUnit::shutdown()
{
	Visualizer::setEnabled(visualizer_enabled);
}

void BodyFractureUnit::onContactEnter(const Unigine::BodyPtr &body, int num)
{
	if (!body)
		return;

	float impulse = body->getContactImpulse(num);
	if (impulse < max_impulse)
		return;

	auto b0 = body->getContactBody0(num);
	auto b1 = body->getContactBody1(num);

	while (b0 && b0->getType() != Body::BODY_FRACTURE)
		b0 = b0->getParent();
	while (b1 && b1->getType() != Body::BODY_FRACTURE)
		b1 = b1->getParent();

	auto bf0 = dynamic_ptr_cast<BodyFracture>(b0);
	auto bf1 = dynamic_ptr_cast<BodyFracture>(b1);
	auto fracture = bf1 ? bf1 : bf0;

	crack(fracture, body->getContactPoint(num), body->getContactNormal(num));

	own_body->getEventContactEnter().disconnect(this, &BodyFractureUnit::onContactEnter);
}

void BodyFractureUnit::crack(const Unigine::BodyFracturePtr &fracture, const Unigine::Math::Vec3 &point, const Unigine::Math::vec3 &normal)
{
	if (!fracture)
		return;

	switch (mode)
	{
	case MODE_SLICING:
		fracture->createSlicePieces(point, normal);
		break;
	case MODE_CRACKING:
		fracture->createCrackPieces(point, normal, cracking_num_cuts, cracking_num_rings, cracking_step);
		break;
	case MODE_SHATTERING:
		fracture->createShatterPieces(shattering_num_pieces);
		break;
	default:
		Log::warning("BodyFractureUnit::onContactEnter(body, int): Unknown fracturing mode %i\n", mode.get());
		break;
	}

	fracture->setBroken(true);
}
