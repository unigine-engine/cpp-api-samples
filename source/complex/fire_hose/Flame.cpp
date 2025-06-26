#include "Flame.h"
#include "ExtinguishingPuddles.h"
#include "UnigineGame.h"
#include "UnigineVisualizer.h"

REGISTER_COMPONENT(Flame);

using namespace Unigine;
using namespace Math;


void Flame::setBurn(bool burn_)
{
	this->burn = burn_;
	if (emitter)
	{
		emitter->setEmitterEnabled(burn_);
	}
}

void Flame::init()
{
	if (emitter_node)
	{
		emitter = checked_ptr_cast<ObjectParticles>(emitter_node.get());
		if (!emitter)
		{
			Log::error("%s: Flame component can only accept ObjectParticles as the emitter_node\n", __FUNCTION__);
			node->setEnabled(false);
		}
	}
	else
	{
		Log::error("%s: Flame component can not found ObjectParticles\n", __FUNCTION__);
		node->setEnabled(false);
	}
}

void Flame::update()
{
	Vector<NodePtr> nodes;
	BoundBox box(vec3(-10.0f), vec3(10.0f));
	
	box.setTransform(node->getWorldTransform());
	World::getIntersection(box, Unigine::Node::DECAL_ORTHO, nodes);

	if ((nodes.size() > 10) == getBurn())
		setBurn(!getBurn());
}
