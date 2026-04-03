// Custom trigger components using math-based bounds checking.
// WorldMathTrigger tests registered objects each frame using point-inside checks.
// WorldIntersectionTrigger uses World::getIntersection for automatic detection.

#pragma once
#include <UnigineComponentSystem.h>
#include <UnigineCallback.h>
#include <UnigineMathLibBounds.h>
#include <UnigineSignal.h>

// Tests registered objects against sphere or box bounds using point-inside checks.
class WorldMathTrigger :
	public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(WorldMathTrigger, ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Sphere radius for collision detection
	PROP_PARAM(Float, bound_sphere_size, 5.0f);
	// Box size for collision detection
	PROP_PARAM(Float, bound_box_size, 5.0f);
	// When true uses sphere, otherwise uses box
	PROP_PARAM(Toggle, is_sphere, false);
	// Enables visual debug bounds rendering
	PROP_PARAM(Toggle, debug, false);


public:
	// Object management - nodes must be registered to be tracked
	void addObject(const Unigine::NodePtr& obj);
	void addObjects(const Unigine::Vector<Unigine::NodePtr>& input_objects);
	int getNumObjects() const { return objects.size(); }
	const Unigine::NodePtr& getObjectsByIndex(int index) const { return objects[index]; }
	void removeObject(const Unigine::NodePtr& obj);
	void removeObjectByIndex(int index) { objects.remove(index); }
	void clearObjects() { objects.clear(); }

	// Callback types for enter/leave events
	enum CALLBACK_TRIGGER
	{
		CALLBACK_TRIGGER_ENTER,
		CALLBACK_TRIGGER_LEAVE,
		CALLBACK_TRIGGER_NUM
	};

	// Callback registration for enter/leave events
	void *addCallback(CALLBACK_TRIGGER callback, Unigine::CallbackBase1<Unigine::NodePtr> *func);
	bool removeCallback(CALLBACK_TRIGGER callback, void *id);
	void clearCallbacks(CALLBACK_TRIGGER callback);

private:
	void init();
	void update();
	void shutdown();

	// Fires enter callback for newly inside objects
	void check_entered();
	// Fires leave callback for exited objects
	void check_leave();

	// Updates bounds position to match node transform
	void replace_bounds();
	void visualize_bounds();

	// Point-inside tests for each shape type
	bool check_sphere(const Unigine::NodePtr &obj) const;
	bool check_box(const Unigine::NodePtr &obj) const;

	inline bool is_callback(CALLBACK_TRIGGER callback) const;
	inline void run_callback(CALLBACK_TRIGGER callback,  const Unigine::NodePtr &obj);

private:
	// Registered callbacks for enter/leave events
	Unigine::Signal signal[CALLBACK_TRIGGER_NUM];

	// Objects being tracked for trigger detection
	Unigine::Vector<Unigine::NodePtr> objects;
	// Objects currently inside the trigger volume
	Unigine::Vector<Unigine::NodePtr> entered;

	Unigine::Math::BoundBox bound_box;
	Unigine::Math::BoundSphere bound_sphere;
};


// Uses World::getIntersection to detect any nodes entering/leaving the volume.
class WorldIntersectionTrigger :
	public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(WorldIntersectionTrigger, ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Sphere radius for intersection queries
	PROP_PARAM(Float, bound_sphere_size, 5.0f);
	// Box size for intersection queries
	PROP_PARAM(Float, bound_box_size, 5.0f);
	// When true uses sphere, otherwise uses box
	PROP_PARAM(Toggle, is_sphere, false);
	// Enables visual debug bounds rendering
	PROP_PARAM(Toggle, debug, false);

public:
	// Callback types for enter/leave events
	enum CALLBACK_TRIGGER
	{
		CALLBACK_TRIGGER_ENTER,
		CALLBACK_TRIGGER_LEAVE,
		CALLBACK_TRIGGER_NUM
	};

	// Callback registration for enter/leave events
	void* addCallback(CALLBACK_TRIGGER callback, Unigine::CallbackBase1<Unigine::NodePtr>* func);
	bool removeCallback(CALLBACK_TRIGGER callback, void* id);
	void clearCallbacks(CALLBACK_TRIGGER callback);

private:
	void init();
	void update();
	void shutdown();

	// Fires enter callback for newly detected nodes
	void check_entered();
	// Fires leave callback for nodes no longer in volume
	void check_leave();

	// Updates bounds position to match node transform
	void replace_bounds();
	void visualize_bounds();

	// Queries world for all nodes inside bounds
	void get_inside_nodes();

	inline bool is_callback(CALLBACK_TRIGGER callback) const;
	inline void run_callback(CALLBACK_TRIGGER callback, const Unigine::NodePtr& obj);

private:
	// Registered callbacks for enter/leave events
	Unigine::Signal signal[CALLBACK_TRIGGER_NUM];

	// Nodes currently tracked as inside
	Unigine::Vector<Unigine::NodePtr> entered;
	// Nodes found inside during current frame query
	Unigine::Vector<Unigine::NodePtr> inside;

	Unigine::Math::BoundBox bound_box;
	Unigine::Math::BoundSphere bound_sphere;

	// Nodes to exclude from detection
	Unigine::Vector<Unigine::NodePtr> exclude;

};

