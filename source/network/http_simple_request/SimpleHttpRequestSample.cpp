#include "SimpleHttpRequestSample.h"

#include <UnigineWidgets.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(SimpleHttpRequestSample);

using namespace Unigine;
using namespace Math;

void SimpleHttpRequestSample::init()
{
	init_gui();

	ThreadQueue::get().run();
}

void SimpleHttpRequestSample::update()
{
	if (city_request.size() && city_request.last()->isCompleted())
	{
		w_list->clear();

		if (httplib::Result &result = city_request.last()->get())
		{
			JsonPtr json = Json::create();
			json->parse(result->body.c_str());
			Log::message("response:\n%s\n", json->getFormattedSubTree().get());
			auto arr = json->getChild("results");
			if (arr && arr->getNumChildren() != 0)
			{
				for (int i = 0; i < arr->getNumChildren(); i++)
				{
					auto child = arr->getChild(i);
					String name = child->getChild("name")->getString();
					if (child->isChild("country"))
						name += ", " + child->getChild("country")->getString();
					if (child->isChild("admin1"))
						name += ", " + child->getChild("admin1")->getString();

					int item = w_list->addItem(name);

					double lat = child->getChild("latitude")->getNumber();
					double lon = child->getChild("longitude")->getNumber();
					String data = String::format("latitude=%f&longitude=%f", lat, lon);

					w_list->setItemData(item, data);
				}
			} else
			{
				w_search->setFontColor(vec4_red);
			}
		} else
		{
			String error = httplib::to_string(result.error()).c_str();
			Log::error("%s\n", error.get());
		}
		city_request.clear();
	}

	if (forecast_request && forecast_request->isCompleted())
	{
		String res;

		if (httplib::Result &result = forecast_request->get())
		{
			JsonPtr json = Json::create();
			json->parse(result->body.c_str());
			Log::message("response:\n%s\n", json->getFormattedSubTree().get());

			res += "Temperature: ";
			res += String::ftoa(toFloat(json->getChild("current")->getChild("temperature_2m")->getNumber()), 1);
			res += json->getChild("current_units")->getChild("temperature_2m")->getString();
			res += "\n";

			res += "Humidity: ";
			res += String::ftoa(toFloat(json->getChild("current")->getChild("relative_humidity_2m")->getNumber()), 1);
			res += json->getChild("current_units")->getChild("relative_humidity_2m")->getString();
			res += "\n";

			res += "Precipitation: ";
			res += String::ftoa(toFloat(json->getChild("current")->getChild("precipitation")->getNumber()), 1);
			res += json->getChild("current_units")->getChild("precipitation")->getString();
			res += "\n";

			res += "Wind Speed: ";
			res += String::ftoa(toFloat(json->getChild("current")->getChild("wind_speed_10m")->getNumber()), 1);
			res += json->getChild("current_units")->getChild("wind_speed_10m")->getString();
			res += "\n";

			res += "Wind Direction: ";
			res += String::ftoa(toFloat(json->getChild("current")->getChild("wind_direction_10m")->getNumber()), 1);
			res += json->getChild("current_units")->getChild("wind_direction_10m")->getString();
			res += "\n";
		} else
		{
			String error = httplib::to_string(result.error()).c_str();
			Log::error("%s\n", error.get());
		}
		sample_description_window.setStatus(res);

		forecast_request.reset();
	}
}

void SimpleHttpRequestSample::shutdown()
{
	ThreadQueue::get().stop();
	sample_description_window.shutdown();
}

void SimpleHttpRequestSample::init_gui()
{
	sample_description_window.createWindow();

	sample_description_window.getWindow()->arrange();
	auto size = Gui::getCurrent()->getSize() / 2 - ivec2(sample_description_window.getWindow()->getWidth(), sample_description_window.getWindow()->getHeight()) / 2;
	sample_description_window.getWindow()->setPosition(size.x, size.y);

	auto gui = Gui::getCurrent();

	auto &parent = sample_description_window.getWindow();

	auto w_group_search = WidgetGroupBox::create("Search", 20, 10);
	parent->addChild(w_group_search, Gui::ALIGN_EXPAND);

	w_search = WidgetEditLine::create(gui);
	w_search->setToolTip("search city by name");
	w_group_search->addChild(w_search, Gui::ALIGN_EXPAND);

	w_search->getEventKeyPressed().connect(*this, [this]()
		{
		w_search->setFontColor(vec4_white);
		String str = "/v1/search?name=";
		str += w_search->getText();

		// create new async request, and place it in array
		// http://geocoding-api.open-meteo.com/v1/search?name=name
		city_request.push_back(std::make_unique<AsyncTask<httplib::Result>>([str]() -> httplib::Result {
				httplib::Client cli("geocoding-api.open-meteo.com");
				return cli.Get(str.get());
			})); });

	w_list = WidgetListBox::create(gui);
	w_group_search->addChild(w_list, Gui::ALIGN_LEFT);
	w_list->getEventChanged().connect(*this, [this]()
		{
			sample_description_window.setStatus("");
			if (w_list->getCurrentItem() != -1)
			{
				sample_description_window.setStatus("request...");
				// create new request
				// api.open-meteo.com/v1/forecast?latitude=12.34&longitude=12.34&current=temperature_2m,relative_humidity_2m,precipitation,wind_speed_10m,wind_direction_10m
				auto request_str = String::format("/v1/forecast?%s&current=temperature_2m,relative_humidity_2m,precipitation,wind_speed_10m,wind_direction_10m", w_list->getCurrentItemData());
				forecast_request = std::make_unique<AsyncTask<httplib::Result>>([request_str]() -> httplib::Result {
					httplib::Client cli("api.open-meteo.com");
					return cli.Get(request_str.get());
				});
			}
		});

	sample_description_window.setStatus("Type city name");
}
