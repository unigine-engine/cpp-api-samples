#pragma once

#include <UnigineThread.h>
#include <UnigineHashMap.h>
#include <UnigineEvent.h>
#include <memory>
#include <atomic>

#include "httplib.h"

class HttpServer: public Unigine::Thread
{
public:
	HttpServer(Unigine::String host_ip, int server_port);
	~HttpServer();

	bool isStopped() const { return stopped; }

	void stop();

	void get(const char *route, std::function<void(const httplib::Request&, httplib::Response&)> handler, bool async = false);

private:
	void process() override;

	void process_tasks();
	Unigine::EventConnection engine_update_connection;

	struct TaskHandler
	{
		TaskHandler(std::function<void(const httplib::Request&, httplib::Response&)> handler, const httplib::Request &req, httplib::Response &res)
			: handler(handler)
			, request(req)
			, response(res)
			, completed(false)
			, canceled(false)
		{}

		std::function<void(const httplib::Request&, httplib::Response&)> handler;
		const httplib::Request &request;
		httplib::Response &response;
		std::atomic<bool> completed{ false };
		std::atomic<bool> canceled{ false };
	};

	Unigine::Vector<std::shared_ptr<TaskHandler>> tasks_queue;
	httplib::Server server;
	std::string host;
	int port;
	Unigine::Mutex tasks_queue_mutex;
	std::atomic<bool> stopped{ true };
};
