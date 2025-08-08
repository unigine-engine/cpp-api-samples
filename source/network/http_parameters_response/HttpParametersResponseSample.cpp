#include "HttpParametersResponseSample.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(HttpParametersResponseSample);

using namespace Unigine;
using namespace Math;

void HttpParametersResponseSample::init_gui()
{
	auto object_text = checked_ptr_cast<ObjectText>(pedestal_text.get());
	object_text->setFontRich(1);
	object_text->setText("<p>Open:</p>http://" + String(host_ip.get()) + ":" + String::itoa(port) + "/set_camera?x=10&amp;y=10");
	auto prev_pos = object_text->getPosition();
	object_text->setPosition(Vec3(object_text->getTextWidth() / 2, prev_pos.y, prev_pos.z));
}

void HttpParametersResponseSample::init()
{
	init_gui();

	server = new HttpServer(host_ip.get(), port.get());
	server->get("/set_camera", [](const httplib::Request& req, httplib::Response& res) {

		auto player = Game::getPlayer();
		auto xString =req.get_param_value("x");
		auto yString = req.get_param_value("y");
	
		double x = String::atod(xString.data());
		double y = String::atod(yString.data());

		if (player)
		{
			Vec3 position = player->getWorldPosition();
			player->setWorldPosition(Vec3(x, y, position.z));

			position = player->getWorldPosition();
			String response = "New player position:\r\n";
			response += "x: " + String::ftoa(position.x) + "\r\n";
			response += "y: " + String::ftoa(position.y) + "\r\n";
			response += "z: " + String::ftoa(position.z) + "\r\n";
			res.set_content(response.get(), "text/plain");
		}
		else
		{
			res.status = 500;
			res.set_content("The player is currently unspecified.", "text/plain");
			Log::error("The player is currently unspecified");
		}
	});
	server->run();
}

void HttpParametersResponseSample::shutdown()
{
	server->stop();
	delete server;
	server = nullptr;
}
