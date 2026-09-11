#include "ExperimentalSeekerCrowd.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(ExperimentalSeekerCrowd);

using namespace Unigine;
using namespace Unigine::Math;

void ExperimentalSeekerCrowd::init()
{
	// the solver may already exist if agents were registered before init
	if (!avoidance)
		avoidance = ExperimentalNavigationAvoidance::create();

	applySettings();
}

void ExperimentalSeekerCrowd::update()
{
	if (!avoidance || agents.size() == 0)
		return;

	float ifps = Game::getIFps();

	// intent phase: each seeker computes its desired velocity
	for (auto *seeker : agents)
		seeker->computeControl();

	for (int i = 0; i < agents.size(); i += 1)
	{
		ExperimentalSeeker *seeker = agents[i];
		avoidance->setAgent(i,
			seeker->getNode()->getWorldPosition(),
			seeker->agent_radius.get(),
			seeker->agent_height.get(),
			seeker->getVelocity(),
			seeker->getDesiredVelocity(),
			seeker->movement_speed,
			seeker->max_acceleration);

		avoidance->setAgentInteractionMask(i, seeker->getAvoidanceInteractionMask());
		if (seeker->getAvoidancePriority() >= 0.0f)
			avoidance->setAgentPriority(i, seeker->getAvoidancePriority());
	}

	// hard de-penetration of already overlapping agents; the corrected
	// positions go through the corridors to stay on the navigation mesh
	avoidance->separateOverlappingAgents();
	for (int i = 0; i < agents.size(); i += 1)
		agents[i]->correctPosition(avoidance->getAgentPosition(i));

	// soft avoidance: move the agents with the adjusted velocities
	avoidance->compute(ifps);
	for (int i = 0; i < agents.size(); i += 1)
		agents[i]->moveWithVelocity(avoidance->getAgentVelocity(i), ifps);
}

void ExperimentalSeekerCrowd::shutdown()
{
	// the seeker components may already be destroyed at world shutdown,
	// the list is dropped without touching them
	agents.clear();
	if (avoidance)
		avoidance->setNumAgents(0);
}

// Pushes the settings to the avoidance solver.
void ExperimentalSeekerCrowd::applySettings()
{
	if (!avoidance)
		return;

	avoidance->setNeighborRange(neighbor_range);
	avoidance->setMaxNeighbors(max_neighbors);
	avoidance->setPredictionTime(prediction_time);
	avoidance->setWeightDesiredVelocity(weight_desired_velocity);
	avoidance->setWeightCurrentVelocity(weight_current_velocity);
	avoidance->setWeightSide(weight_side);
	avoidance->setWeightTimeToImpact(weight_time_to_impact);
	avoidance->setWeightSeparation(weight_separation);
}

void ExperimentalSeekerCrowd::addAgent(ExperimentalSeeker *seeker)
{
	if (!seeker || agents.contains(seeker))
		return;

	// agents may be registered before init, e.g. from another component
	if (!avoidance)
		avoidance = ExperimentalNavigationAvoidance::create();

	seeker->setControlledExternally(true);
	agents.append(seeker);
	avoidance->setNumAgents(agents.size());
}

void ExperimentalSeekerCrowd::removeAgent(ExperimentalSeeker *seeker)
{
	int index = agents.findIndex(seeker);
	if (index == -1)
		return;

	seeker->setControlledExternally(false);
	agents.remove(index);
	avoidance->setNumAgents(agents.size());
}

void ExperimentalSeekerCrowd::clearAgents()
{
	for (auto *seeker : agents)
		seeker->setControlledExternally(false);
	agents.clear();

	if (avoidance)
		avoidance->setNumAgents(0);
}
