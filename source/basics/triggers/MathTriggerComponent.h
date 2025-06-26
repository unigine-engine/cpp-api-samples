#pragma once
#include <UnigineComponentSystem.h>
#include <UnigineCallback.h>
#include <UnigineMathLibBounds.h>
#include <UnigineSignal.h>

class WorldMathTrigger :
	public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(WorldMathTrigger, ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Float, bound_sphere_size, 5.0f);
	PROP_PARAM(Float, bound_box_size, 5.0f);
	PROP_PARAM(Toggle, is_sphere, false);
	PROP_PARAM(Toggle, debug, false);


public:

	void addObject(const Unigine::NodePtr& obj);
	void addObjects(const Unigine::Vector<Unigine::NodePtr>& input_objects);
	int getNumObjects() const { return objects.size(); }
	const Unigine::NodePtr& getObjectsByIndex(int index) const { return objects[index]; }
	void removeObject(const Unigine::NodePtr& obj);
	void removeObjectByIndex(int index) { objects.remove(index); }
	void clearObjects() { objects.clear(); }

	enum CALLBACK_TRIGGER
	{
		CALLBACK_TRIGGER_ENTER,
		CALLBACK_TRIGGER_LEAVE,
		CALLBACK_TRIGGER_NUM
	};

	void *addCallback(CALLBACK_TRIGGER callback, Unigine::CallbackBase1<Unigine::NodePtr> *func);
	bool removeCallback(CALLBACK_TRIGGER callback, void *id);
	void clearCallbacks(CALLBACK_TRIGGER callback);

private:
	void init();
	void update();
	void shutdown();

	void check_entered();
	void check_leave();

	void replace_bounds();
	void visualize_bounds();

	bool check_sphere(const Unigine::NodePtr &obj) const;
	bool check_box(const Unigine::NodePtr &obj) const;

	inline bool is_callback(CALLBACK_TRIGGER callback) const;
	inline void run_callback(CALLBACK_TRIGGER callback,  const Unigine::NodePtr &obj);

private:
	Unigine::Signal signal[CALLBACK_TRIGGER_NUM];

	Unigine::Vector<Unigine::NodePtr> objects;
	Unigine::Vector<Unigine::NodePtr> entered;

	Unigine::Math::BoundBox bound_box;
	Unigine::Math::BoundSphere bound_sphere;
};


class WorldIntersectionTrigger :
	public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(WorldIntersectionTrigger, ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Float, bound_sphere_size, 5.0f);
	PROP_PARAM(Float, bound_box_size, 5.0f);
	PROP_PARAM(Toggle, is_sphere, false);
	PROP_PARAM(Toggle, debug, false);

public:
	enum CALLBACK_TRIGGER
	{
		CALLBACK_TRIGGER_ENTER,
		CALLBACK_TRIGGER_LEAVE,
		CALLBACK_TRIGGER_NUM
	};

	void* addCallback(CALLBACK_TRIGGER callback, Unigine::CallbackBase1<Unigine::NodePtr>* func);
	bool removeCallback(CALLBACK_TRIGGER callback, void* id);
	void clearCallbacks(CALLBACK_TRIGGER callback);

private:
	void init();
	void update();
	void shutdown();

	void check_entered();
	void check_leave();

	void replace_bounds();
	void visualize_bounds();

	void get_inside_nodes();

	inline bool is_callback(CALLBACK_TRIGGER callback) const;
	inline void run_callback(CALLBACK_TRIGGER callback, const Unigine::NodePtr& obj);

private:
	Unigine::Signal signal[CALLBACK_TRIGGER_NUM];

	Unigine::Vector<Unigine::NodePtr> entered;
	Unigine::Vector<Unigine::NodePtr> inside;

	Unigine::Math::BoundBox bound_box;
	Unigine::Math::BoundSphere bound_sphere;

	Unigine::Vector<Unigine::NodePtr> exclude;

};

