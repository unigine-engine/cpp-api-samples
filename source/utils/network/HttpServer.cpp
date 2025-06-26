#include "HttpServer.h"

#include <UnigineEngine.h>

using namespace Unigine;

HttpServer::HttpServer(String host_ip, int server_port)
	: host(host_ip.get())
	, port(server_port)
{
	server.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
		res.set_header("Access-Control-Allow-Origin", "*");
		res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
		res.set_header("Access-Control-Allow-Headers", "Content-Type");
		return httplib::Server::HandlerResponse::Unhandled;
	});

	Engine::get()->getEventEndUpdate().connect(engine_update_connection, this, &HttpServer::process_tasks);
}

HttpServer::~HttpServer() = default;

void HttpServer::process()
{
	stopped = false;
	server.listen(host, port);
}

void HttpServer::process_tasks()
{
	// sync tasks should be executed only in main thread
	if (!Engine::get()->isMainThread())
		return;

	ScopedLock lock{ tasks_queue_mutex };
	for (int i = 0; i < tasks_queue.size(); ++i)
	{
		auto task = tasks_queue[i];
		task->handler(task->request, task->response);
		task->completed = true;
	}
	tasks_queue.clear();
}

void HttpServer::stop()
{
	server.stop();
	stopped = true;
	Thread::stop();
}

void HttpServer::get(const char* route, std::function<void(const httplib::Request &req, httplib::Response &res)> handler, bool async)
{
	server.Get(route, [this, handler, async](const httplib::Request &req, httplib::Response &res) {
		auto task = std::make_shared<TaskHandler>(handler, req, res);
		if (async)
		{
			task->handler(req, res);
			task->completed = true;
		}
		else
		{
			ScopedLock lock{ tasks_queue_mutex };
			tasks_queue.push_back(task);
		}

		// waiting for sync task complete
		while (!task->completed && !task->canceled)
		{
			Thread::switchThread();
		}

		task.reset();
	});
}