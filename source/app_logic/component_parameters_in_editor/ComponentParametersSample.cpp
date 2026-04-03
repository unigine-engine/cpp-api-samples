// Skeleton sample demonstrating the Component System lifecycle methods. Shows all
// available callbacks: init, update_async_thread, update_sync_thread, update,
// post_update, update_physics, swap, and shutdown. Actual logic is defined in header.

#include "ComponentParametersSample.h"

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(ComponentParameters);

// Called on world initialization; resources for the world scene are initialized here.
void ComponentParameters::init()
{

}

// Called before updateSyncThread in parallel threads; runs alongside postUpdate and updatePhysics.
// Suitable for heavy calculations (pathfinding, procedural textures) that do not block the main thread.
void ComponentParameters::update_async_thread()
{

}

// Called before update and postUpdate in parallel threads; blocks the main thread until complete.
// Suitable for complex calculations to be applied to the current node with more safety guarantees.
void ComponentParameters::update_sync_thread()
{

}

// Called before each render frame; all logic-related functions are specified here.
void ComponentParameters::update()
{

}

// Called before rendering each frame; behavior can be corrected after node state is updated.
void ComponentParameters::post_update()
{

}

// Called at fixed physics rate (60 times per second by default) regardless of frame rate.
void ComponentParameters::update_physics()
{

}

// Called after rendering, physics, pathfinding, GUI, and all async threads complete.
// Results of updateAsyncThread can be safely processed here before deletion queue runs.
void ComponentParameters::swap()
{

}

// Called on world shutdown; resources created during execution are deleted here to avoid memory leaks.
void ComponentParameters::shutdown()
{

}
