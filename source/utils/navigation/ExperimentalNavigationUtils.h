#pragma once

#include <UnigineExperimentalNavigation.h>

namespace Utils
{

// human readable names of the route query results, e.g. for the sample UI

UNIGINE_INLINE const char *getRouteStatusName(Unigine::ExperimentalNavigationPath::STATUS status)
{
	switch (status)
	{
		case Unigine::ExperimentalNavigationPath::STATUS_COMPLETE: return "complete";
		case Unigine::ExperimentalNavigationPath::STATUS_PARTIAL: return "partial";
		default: return "no path";
	}
}

UNIGINE_INLINE const char *getRouteFailureName(Unigine::ExperimentalNavigationPath::FAILURE failure)
{
	switch (failure)
	{
		case Unigine::ExperimentalNavigationPath::FAILURE_NONE: return "none";
		case Unigine::ExperimentalNavigationPath::FAILURE_START_OUTSIDE: return "start outside the mesh";
		case Unigine::ExperimentalNavigationPath::FAILURE_TARGET_OUTSIDE: return "target outside the mesh";
		case Unigine::ExperimentalNavigationPath::FAILURE_DISCONNECTED: return "disconnected";
		case Unigine::ExperimentalNavigationPath::FAILURE_NO_FILTER: return "no filter";
		case Unigine::ExperimentalNavigationPath::FAILURE_NO_MATCHING_MESH: return "no matching mesh";
		case Unigine::ExperimentalNavigationPath::FAILURE_POLYGON_MESH_MISMATCH: return "polygon mesh mismatch";
		case Unigine::ExperimentalNavigationPath::FAILURE_DATA_MISSING: return "data missing";
		case Unigine::ExperimentalNavigationPath::FAILURE_SEARCH_NODES_LIMIT: return "search nodes limit";
		case Unigine::ExperimentalNavigationPath::FAILURE_COST_LIMIT: return "cost limit";
		case Unigine::ExperimentalNavigationPath::FAILURE_POLYGON_LIMIT: return "polygon limit";
		case Unigine::ExperimentalNavigationPath::FAILURE_OBSTACLE_BLOCKED: return "blocked by an obstacle";
		case Unigine::ExperimentalNavigationPath::FAILURE_NO_AVAILABLE_MESH: return "no available mesh";
		case Unigine::ExperimentalNavigationPath::FAILURE_NAVIGATION_MASK_MISMATCH: return "navigation mask mismatch";
		default: return "unknown";
	}
}

} // namespace Utils
