#pragma once

#include <UnigineWorld.h>
#include <UnigineThread.h>
#include <UnigineSignal.h>
#include <functional>

////////////////////////////////////////////////////////
// Intersection Request
////////////////////////////////////////////////////////

class IntersectionRequest;

class IntersectionRequestCPUShader: public Unigine::CPUShader
{
public:
	IntersectionRequestCPUShader(Unigine::Vector<IntersectionRequest *> in_requests, bool in_force)
		: requests(in_requests), is_force(in_force) {}

	virtual ~IntersectionRequestCPUShader() { wait(); }

	void process(int thread_num, int threads_count) override;
	void run(int threads_count);

private:
	Unigine::Vector<IntersectionRequest *> requests;
	bool is_force{false};
	volatile int counter{0};
};

class IntersectionRequest : public Unigine::EventConnections
{
public:

	enum
	{
		ASYNC_MODE_IMMEDIATELY = 0,
		ASYNC_MODE_RENDER_BEGIN,
	};

	IntersectionRequest(int async_world_check = ASYNC_MODE_IMMEDIATELY);

	// async
	bool isAsyncCompleted();

	long long getAsyncBeginFrame() const { return async_begin_frame; }
	long long getAsyncEndFrame() const { return async_end_frame; }
	long long getAsyncFrameLatency() const { return async_end_frame - async_begin_frame; }

	void *addAsyncEndCallback(Unigine::CallbackBase *func) { return async_end_signal.add(func); }
	void removeAsyncEndCallback(void *id) { async_end_signal.remove(id); }

	void wait();

	// parameters
	void setPositionBegin(const Unigine::Math::Vec3 &pos) { position_begin = pos; }
	Unigine::Math::Vec3 getPositionBegin() const { return position_begin; }

	void setPositionEnd(const Unigine::Math::Vec3 &pos) { position_end = pos; }
	Unigine::Math::Vec3 getPositionEnd() const { return position_end; }

	void setMask(int in_mask) { mask = in_mask; }
	int getMask() const { return mask; }

	void setExcludeNodes(const Unigine::Vector<Unigine::NodePtr> &nodes) { exclude = nodes; }
	void clearExcludeNodes() { exclude.clear(); }

	// results
	bool isIntersection();
	Unigine::NodePtr getNode();
	Unigine::Math::Vec3 getPoint();
	Unigine::Math::vec3 getNormal();

	// requests
	bool getForce();
	bool getForce(const Unigine::Math::Vec3 &p0, const Unigine::Math::Vec3 &p1);
	bool getForce(const Unigine::Math::Vec3 &p0, const Unigine::Math::Vec3 &p1, int mask);
	bool getForce(const Unigine::Math::Vec3 &p0, const Unigine::Math::Vec3 &p1, int mask, const Unigine::Vector<Unigine::NodePtr> &exclude_nodes);

	void getAsync(bool is_critical = false);
	void getAsync(const Unigine::Math::Vec3 &p0, const Unigine::Math::Vec3 &p1, bool is_critical = false);
	void getAsync(const Unigine::Math::Vec3 &p0, const Unigine::Math::Vec3 &p1, int mask, bool is_critical = false);
	void getAsync(const Unigine::Math::Vec3 &p0, const Unigine::Math::Vec3 &p1, int mask, const Unigine::Vector<Unigine::NodePtr> &exclude_nodes, bool is_critical = false);

private:
	// internal
	void on_engine_begin_render();

	void async_intersection();
	void on_landscape_async_end();

private:
	friend class Intersections;

	Unigine::WorldIntersectionNormalPtr world_intersection;
	Unigine::LandscapeFetchPtr landscape_fetch;

	enum
	{
		ASYNC_NOT_USED = 0,
		ASYNC_PREPARE_STAGE,
		ASYNC_PROCESSING_STAGE,
		ASYNC_IS_COMPLETED,
	};

	struct Flags
	{
		unsigned char async_mode : 1;
		unsigned char async_stage : 2;
		unsigned char is_critical_async : 1;
		unsigned char is_world_intersection : 1;
	};

	Flags flags;

	// async
	Unigine::EventConnection on_begin_render_id;

	long long async_begin_frame{0};
	long long async_end_frame{0};

	Unigine::Signal async_end_signal;

	// parameters
	Unigine::Math::Vec3 position_begin;
	Unigine::Math::Vec3 position_end;
	int mask{1};
	Unigine::Vector<Unigine::NodePtr> exclude;

	// results
	Unigine::NodePtr world_node;
};

////////////////////////////////////////////////////////
// Intersections
////////////////////////////////////////////////////////

class Intersections
{
private:
	Intersections() {}

public:
	~Intersections();

	Intersections(const Intersections &) = delete;
	void operator=(const Intersections &) = delete;

	enum
	{
		INVOKE_STAGE_UPDATE = 0,
		INVOKE_STAGE_RENDER,
		INVOKE_STAGE_SWAP,
	};

	// force
	static bool getForce(const Unigine::Math::Vec3 &p0, const Unigine::Math::Vec3 &p1, int mask);
	static bool getForce(const Unigine::Math::Vec3 &p0, const Unigine::Math::Vec3 &p1, int mask, const Unigine::Vector<Unigine::NodePtr> &exclude_nodes);

	static bool isForceIntersection() { return get().force_request.isIntersection(); }
	static Unigine::NodePtr getForceNode() { return get().force_request.getNode(); }
	static Unigine::Math::Vec3 getForcePoint() { return get().force_request.getPoint(); }
	static Unigine::Math::vec3 getForceNormal() { return get().force_request.getNormal(); }

	// async
	static void *getAsync(const Unigine::Math::Vec3 &p0, const Unigine::Math::Vec3 &p1, int mask, Unigine::CallbackBase1<IntersectionRequest *> *func, int invoke_stage = INVOKE_STAGE_UPDATE, bool is_critical = false);
	static void *getAsync(const Unigine::Math::Vec3 &p0, const Unigine::Math::Vec3 &p1, int mask, const Unigine::Vector<Unigine::NodePtr> &exclude_nodes,
						  Unigine::CallbackBase1<IntersectionRequest *> *func, int invoke_stage = INVOKE_STAGE_UPDATE, bool is_critical = false);

	static void *getAsync(const Unigine::Math::Vec3 &p0, const Unigine::Math::Vec3 &p1, int mask, const std::function<void(IntersectionRequest *)> func, int invoke_stage = INVOKE_STAGE_UPDATE, bool is_critical = false)
	{
		return getAsync(p0, p1, mask, Unigine::MakeCallback(func), invoke_stage, is_critical);
	}

	static void *getAsync(const Unigine::Math::Vec3 &p0, const Unigine::Math::Vec3 &p1, int mask, const Unigine::Vector<Unigine::NodePtr> &exclude_nodes,
						  const std::function<void(IntersectionRequest *)> func, int invoke_stage = INVOKE_STAGE_UPDATE, bool is_critical = false)
	{
		return getAsync(p0, p1, mask, exclude_nodes, Unigine::MakeCallback(func), invoke_stage, is_critical);
	}

	// shaders
	static void setThreadsCount(int count) { get().threads_count = count; }
	static int getThreadsCount() { return get().threads_count; }

	static void getForce(const Unigine::Vector<IntersectionRequest *> &requests);
	static void getAsync(const Unigine::Vector<IntersectionRequest *> &requests);
	static void wait(const Unigine::Vector<IntersectionRequest *> &requests);

private:
	static Intersections &get()
	{
		static Intersections instance;
		return instance;
	}

	void set_callbacks();

	struct AsyncData;
	static AsyncData *grab_request_data();
	static void check_request(AsyncData *data);

	static void on_engine_update();
	static void on_engine_render();
	static void on_engine_swap();

private:
	IntersectionRequest force_request;

	struct AsyncData
	{
		int invoke_stage = INVOKE_STAGE_UPDATE;
		bool is_free{false};
		IntersectionRequest *request{nullptr};
		Unigine::CallbackBase1<IntersectionRequest *> *func{nullptr};
	};

	Unigine::Vector<AsyncData> async_requests;
	Unigine::EventConnections connections;
	int threads_count{-1};
};
