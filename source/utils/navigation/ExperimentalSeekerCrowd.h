#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineExperimentalNavigation.h>

#include "ExperimentalSeeker.h"

// Drives the registered ExperimentalSeeker agents through the avoidance
// solver: gathers their desired velocities, solves and moves the agents with
// the adjusted ones. The agents are registered by the world logic.
class ExperimentalSeekerCrowd : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ExperimentalSeekerCrowd, Unigine::ComponentBase);
	// after the seekers, so their routes are already updated
	COMPONENT_INIT(init, 1);
	COMPONENT_UPDATE(update, 1);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_TOOLTIP("Range within which the agents react to each other; keep above 2 * (speed * prediction time + radius)");
	PROP_PARAM(Float, neighbor_range, 6.f);
	PROP_TOOLTIP("Nearest neighbors an agent reacts to; raise for dense crowds");
	PROP_PARAM(Int, max_neighbors, 6);
	PROP_TOOLTIP("How far ahead the collisions are predicted; higher - more cautious agents");
	PROP_PARAM(Float, prediction_time, 0.4f);
	PROP_TOOLTIP("Pull toward the route intent; higher - more assertive agents");
	PROP_PARAM(Float, weight_desired_velocity, 2.f);
	PROP_TOOLTIP("Pull toward the current velocity; higher - smoother, but standing agents start slower");
	PROP_PARAM(Float, weight_current_velocity, 0.75f);
	PROP_TOOLTIP("Preference of a passing side; helps to resolve the head-on encounters");
	PROP_PARAM(Float, weight_side, 0.75f);
	PROP_TOOLTIP("Penalty of the imminent collisions; higher - the agents brake earlier");
	PROP_PARAM(Float, weight_time_to_impact, 2.5f);
	PROP_TOOLTIP("Push apart of the almost touching agents");
	PROP_PARAM(Float, weight_separation, 2.f);

	// pushes the settings to the avoidance solver, call after changing them at runtime
	void applySettings();

	// agent registration; the crowd takes over the movement control of a
	// registered agent. An agent must be removed before its node is deleted,
	// the crowd holds a raw pointer to the component
	void addAgent(ExperimentalSeeker *seeker);
	void removeAgent(ExperimentalSeeker *seeker);
	void clearAgents();
	int getNumAgents() const { return agents.size(); }
	ExperimentalSeeker *getAgent(int num) const { return agents[num]; }

	// direct access to the avoidance solver, e.g. for density queries
	const Unigine::ExperimentalNavigationAvoidancePtr &getAvoidance() const { return avoidance; }

private:
	void init();
	void update();
	void shutdown();

	Unigine::ExperimentalNavigationAvoidancePtr avoidance;
	Unigine::Vector<ExperimentalSeeker *> agents;
};
