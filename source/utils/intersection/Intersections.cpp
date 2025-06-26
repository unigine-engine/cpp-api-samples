#include "Intersections.h"
#include <UnigineEngine.h>

using namespace Unigine;
using namespace Math;

////////////////////////////////////////////////////////
// Intersection Request
////////////////////////////////////////////////////////

void IntersectionRequestCPUShader::process(int thread_num, int threads_count)
{
	while (true)
	{
		int num = AtomicAdd(&counter, 1);
		if (num >= requests.size())
			return;

		if (is_force)
			requests[num]->getForce();
		else
			requests[num]->getAsync();
	}
}

void IntersectionRequestCPUShader::run(int threads_count)
{
	counter = 0;
	runSync(threads_count);
}

IntersectionRequest::IntersectionRequest(int async_world_check)
{
	flags.async_mode = async_world_check;
	flags.async_stage = ASYNC_IS_COMPLETED;
	flags.is_critical_async = false;
	flags.is_world_intersection = false;

	world_intersection = WorldIntersectionNormal::create();

	landscape_fetch = LandscapeFetch::create();
	landscape_fetch->setUses(0);
	landscape_fetch->setUsesNormal(true);
	landscape_fetch->getEventEnd().connect(this, &IntersectionRequest::on_landscape_async_end);


	if (flags.async_mode == ASYNC_MODE_RENDER_BEGIN)
			Engine::get()->getEventBeginRender().connect(on_begin_render_id, this, &IntersectionRequest::on_engine_begin_render);
}

// async
bool IntersectionRequest::isAsyncCompleted()
{
	if (flags.async_stage == ASYNC_PROCESSING_STAGE && landscape_fetch->isAsyncCompleted())
		flags.async_stage = ASYNC_IS_COMPLETED;

	return flags.async_stage == ASYNC_IS_COMPLETED;
}

void IntersectionRequest::wait()
{
	if (flags.async_mode == ASYNC_MODE_RENDER_BEGIN && flags.async_stage != ASYNC_PROCESSING_STAGE)
		async_intersection();

	landscape_fetch->wait();
	flags.async_stage = ASYNC_IS_COMPLETED;
}

// results
bool IntersectionRequest::isIntersection()
{
	if (flags.async_stage != ASYNC_NOT_USED)
		return (flags.is_world_intersection || landscape_fetch->isIntersection()) && isAsyncCompleted();

	return flags.is_world_intersection;
}

NodePtr IntersectionRequest::getNode()
{
	if (flags.async_stage != ASYNC_NOT_USED && isAsyncCompleted() && landscape_fetch->isIntersection())
		return Landscape::getActiveTerrain();

	return world_node;
}

Vec3 IntersectionRequest::getPoint()
{
	if (flags.async_stage != ASYNC_NOT_USED && isAsyncCompleted() && landscape_fetch->isIntersection())
		return landscape_fetch->getPosition();

	if (flags.is_world_intersection)
		return world_intersection->getPoint();

	return Vec3_zero;
}

vec3 IntersectionRequest::getNormal()
{
	if (flags.async_stage != ASYNC_NOT_USED && isAsyncCompleted() && landscape_fetch->isIntersection())
		return landscape_fetch->getNormal();

	if (flags.is_world_intersection)
		return world_intersection->getNormal();

	return vec3_up;
}

// requests
bool IntersectionRequest::getForce()
{
	// update async
	flags.async_stage = ASYNC_NOT_USED;

	// clear results
	flags.is_world_intersection = false;
	world_node = nullptr;

	// check intersection
	world_node = World::getIntersection(position_begin, position_end, mask, exclude, world_intersection);
	if (world_node)
		flags.is_world_intersection = true;

	return flags.is_world_intersection;
}

bool IntersectionRequest::getForce(const Vec3 &p0, const Vec3 &p1)
{
	position_begin = p0;
	position_end = p1;
	return getForce();
}

bool IntersectionRequest::getForce(const Vec3 &p0, const Vec3 &p1, int in_mask)
{
	mask = in_mask;
	return getForce(p0, p1);
}

bool IntersectionRequest::getForce(const Vec3 &p0, const Vec3 &p1, int in_mask, const Vector<NodePtr> &exclude_nodes)
{
	exclude = exclude_nodes;
	return getForce(p0, p1, in_mask);
}

void IntersectionRequest::getAsync(bool is_critical)
{
	// update async
	flags.async_stage = ASYNC_PREPARE_STAGE;
	flags.is_critical_async = is_critical;
	async_begin_frame = Engine::get()->getFrame();

	// clear results
	flags.is_world_intersection = false;
	world_node = nullptr;

	if (flags.async_mode == ASYNC_MODE_RENDER_BEGIN)
		return;

	async_intersection();
}

void IntersectionRequest::getAsync(const Vec3 &p0, const Vec3 &p1, bool is_critical)
{
	position_begin = p0;
	position_end = p1;
	getAsync(is_critical);
}

void IntersectionRequest::getAsync(const Vec3 &p0, const Vec3 &p1, int in_mask, bool is_critical)
{
	mask = in_mask;
	getAsync(p0, p1, is_critical);
}

void IntersectionRequest::getAsync(const Vec3 &p0, const Vec3 &p1, int in_mask, const Vector<NodePtr> &exclude_nodes, bool is_critical)
{
	exclude = exclude_nodes;
	getAsync(p0, p1, in_mask, is_critical);
}

// internal
void IntersectionRequest::on_engine_begin_render()
{
	if (flags.async_mode != ASYNC_MODE_RENDER_BEGIN)
		return;

	// we can use wait after getAsync
	// in this case invoke immediately world check
	if (flags.async_stage != ASYNC_PREPARE_STAGE)
		return;

	async_intersection();
}

void IntersectionRequest::async_intersection()
{
	bool is_terrain_exluded = exclude.contains(Landscape::getActiveTerrain());
	if (!is_terrain_exluded)
		exclude.append(Landscape::getActiveTerrain());

	flags.async_stage = ASYNC_PROCESSING_STAGE;

	world_node = World::getIntersection(position_begin, position_end, mask, exclude, world_intersection);
	if (world_node)
		flags.is_world_intersection = true;

	if (!is_terrain_exluded)
	{
		exclude.removeOne(Landscape::getActiveTerrain());
		landscape_fetch->intersectionAsync(position_begin, flags.is_world_intersection ? world_intersection->getPoint() : position_end, flags.is_critical_async);
	} else
	{
		flags.async_stage = ASYNC_IS_COMPLETED;
		on_landscape_async_end();
	}
}

void IntersectionRequest::on_landscape_async_end()
{
	async_end_frame = Engine::get()->getFrame();
	async_end_signal.invoke(this);
}

////////////////////////////////////////////////////////
// Intersections
////////////////////////////////////////////////////////

Intersections::~Intersections()
{
	for (int i = 0; i < async_requests.size(); i++)
	{
		async_requests[i].request->landscape_fetch->wait();
		delete async_requests[i].request;
		delete async_requests[i].func;
	}
	async_requests.clear();
}

bool Intersections::getForce(const Vec3 &p0, const Vec3 &p1, int mask)
{
	return get().force_request.getForce(p0, p1, mask);
}

bool Intersections::getForce(const Vec3 &p0, const Vec3 &p1, int mask, const Vector<NodePtr> &exclude_nodes)
{
	return get().force_request.getForce(p0, p1, mask, exclude_nodes);
}

void *Intersections::getAsync(const Vec3 &p0, const Vec3 &p1, int mask, CallbackBase1<IntersectionRequest *> *func, int invoke_stage, bool is_critical)
{
	get().set_callbacks();
	AsyncData *data = grab_request_data();

	data->invoke_stage = invoke_stage;
	data->func = func;
	data->request->getAsync(p0, p1, mask, is_critical);

	return data->request;
}

void *Intersections::getAsync(const Vec3 &p0, const Vec3 &p1, int mask, const Vector<NodePtr> &exclude_nodes, CallbackBase1<IntersectionRequest *> *func, int invoke_stage, bool is_critical)
{
	get().set_callbacks();
	AsyncData *data = grab_request_data();

	data->invoke_stage = invoke_stage;
	data->func = func;
	data->request->getAsync(p0, p1, mask, exclude_nodes, is_critical);

	return data->request;
}

void Intersections::getForce(const Vector<IntersectionRequest *> &requests)
{
	IntersectionRequestCPUShader shader(requests, true);
	shader.run(get().threads_count);
}

void Intersections::getAsync(const Unigine::Vector<IntersectionRequest *> &requests)
{
	IntersectionRequestCPUShader shader(requests, false);
	shader.run(get().threads_count);
}

void Intersections::wait(const Unigine::Vector<IntersectionRequest *> &requests)
{
	for (auto r : requests)
		r->wait();
}

void Intersections::set_callbacks()
{
	if (!connections.empty())
		return;

	Engine::get()->getEventBeginUpdate().connect(connections, &Intersections::on_engine_update);
	Engine::get()->getEventBeginRender().connect(connections, &Intersections::on_engine_render);
	Engine::get()->getEventBeginSwap().connect(connections, &Intersections::on_engine_swap);
}

Intersections::AsyncData *Intersections::grab_request_data()
{
	Vector<AsyncData> &async_requests = get().async_requests;

	AsyncData *data = nullptr;
	for (int i = 0; i < async_requests.size(); i++)
	{
		if (async_requests[i].is_free)
		{
			data = &async_requests[i];
			data->is_free = false;
			break;
		}
	}

	if (!data)
	{
		data = &async_requests.append();
		data->request = new IntersectionRequest(IntersectionRequest::ASYNC_MODE_RENDER_BEGIN);
	}

	return data;
}

void Intersections::check_request(Intersections::AsyncData *data)
{
	if (!data)
		return;

	if (!data->is_free && data->request->isAsyncCompleted())
	{
		if (data->func)
			data->func->run(data->request);

		data->is_free = true;
		delete data->func;
		data->func = nullptr;
	}
}

void Intersections::on_engine_update()
{
	Vector<AsyncData> &async_requests = get().async_requests;

	for (int i = 0; i < async_requests.size(); i++)
	{
		if (async_requests[i].invoke_stage == INVOKE_STAGE_UPDATE)
			check_request(&async_requests[i]);
	}
}

void Intersections::on_engine_render()
{
	Vector<AsyncData> &async_requests = get().async_requests;

	for (int i = 0; i < async_requests.size(); i++)
	{
		if (async_requests[i].invoke_stage == INVOKE_STAGE_RENDER)
			check_request(&async_requests[i]);
	}
}

void Intersections::on_engine_swap()
{
	Vector<AsyncData> &async_requests = get().async_requests;

	for (int i = 0; i < async_requests.size(); i++)
	{
		if (async_requests[i].invoke_stage == INVOKE_STAGE_SWAP)
			check_request(&async_requests[i]);
	}
}
