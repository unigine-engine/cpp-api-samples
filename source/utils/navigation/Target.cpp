#include "Target.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(PathfindingTarget);

using namespace Unigine;
using namespace Unigine::Math;

void PathfindingTarget::init()
{
	take_new_position();
}

void PathfindingTarget::onReached()
{
	take_new_position();
}

void PathfindingTarget::take_new_position()
{
	if (!positions)
		return;
	
	int positions_cnt = positions->getNumChildren();
	
	if (positions_cnt != 0)
	{
		auto new_position = positions->getChild(Game::getRandomInt(0, positions_cnt))->getWorldPosition();
		node->setWorldPosition(new_position);
	}
}
