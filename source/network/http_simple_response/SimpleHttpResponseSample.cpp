#include "SimpleHttpResponseSample.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(SimpleHttpResponseSample);

using namespace Unigine;
using namespace Math;

void SimpleHttpResponseSample::init_gui()
{
	auto object_text = checked_ptr_cast<ObjectText>(pedestal_text.get());
	object_text->setFontRich(1);
	object_text->setText("<p>Open:</p><p>http://" + String(host_ip.get()) + ":" + String::itoa(port) + "/player_pos</p>");
	auto prev_pos = object_text->getPosition();
	object_text->setPosition(Vec3(object_text->getTextWidth() / 2, prev_pos.y, prev_pos.z));
}

void SimpleHttpResponseSample::init()
{
	init_gui();

	server = new HttpServer(host_ip.get(), port.get());

	server->get("/player_pos", [](const httplib::Request& req, httplib::Response& res) {
		// This function will be called from main thread because it is not marked as asynchronous
		auto player = Game::getPlayer();

		if (player)
		{
			Vec3 position = player->getWorldPosition();
			String response = "Player position:\r\n";
			response += "x: " + String::ftoa(position.x) + "\r\n";
			response += "y: " + String::ftoa(position.y) + "\r\n";
			response += "z: " + String::ftoa(position.z) + "\r\n";
			res.set_content(response.get(), "text/plain");
		}
		else
		{
			res.status = 500;
			res.set_content("The player is currently unspecified.", "text/plain");
		}
	});

	server->run();

}

void SimpleHttpResponseSample::shutdown()
{
	server->stop();
	delete server;
	server = nullptr;
}
