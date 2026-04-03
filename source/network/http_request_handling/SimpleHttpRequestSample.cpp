// Demonstrates asynchronous HTTP requests using cpp-httplib library.
// Uses a weather API to search for cities and fetch forecasts.
// Requests run on background threads via AsyncTask to avoid blocking rendering.

#include "SimpleHttpRequestSample.h"

#include <UnigineWidgets.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(SimpleHttpRequestSample);

using namespace Unigine;
using namespace Math;

// GUI is created and background thread queue is started.
void SimpleHttpRequestSample::init()
{
	init_gui();

	// Background thread pool is started for async HTTP requests
	ThreadQueue::get().run();
}

// Pending HTTP requests are polled; responses are parsed and displayed.
void SimpleHttpRequestSample::update()
{
	// City search request completion is checked (non-blocking poll)
	if (city_request.size() && city_request.last()->isCompleted())
	{
		w_list->clear();

		// Result is retrieved from the completed async task
		if (httplib::Result &result = city_request.last()->get())
		{
			// JSON response from geocoding API is parsed
			JsonPtr json = Json::create();
			json->parse(result->body.c_str());
			Log::message("response:\n%s\n", json->getFormattedSubTree().get());
			auto arr = json->getChild("results");
			if (arr && arr->getNumChildren() != 0)
			{
				// Each city result is added to the list with coordinates stored as item data
				for (int i = 0; i < arr->getNumChildren(); i++)
				{
					auto child = arr->getChild(i);
					if (!child->isChild("name") || !child->isChild("latitude") || !child->isChild("longitude"))
						continue;
					String name = child->getChild("name")->getString();
					if (child->isChild("country"))
						name += ", " + child->getChild("country")->getString();
					if (child->isChild("admin1"))
						name += ", " + child->getChild("admin1")->getString();

					int item = w_list->addItem(name);

					// Coordinates are stored as URL query parameters for forecast request
					double lat = child->getChild("latitude")->getNumber();
					double lon = child->getChild("longitude")->getNumber();
					String data = String::format("latitude=%f&longitude=%f", lat, lon);

					w_list->setItemData(item, data);
				}
			} else
			{
				// No results found - input field is highlighted in red
				w_search->setFontColor(vec4_red);
			}
		} else
		{
			// HTTP client error is logged
			String error = httplib::to_string(result.error()).c_str();
			Log::error("%s\n", error.get());
		}
		city_request.clear();
	}

	// Forecast request completion is checked
	if (forecast_request && forecast_request->isCompleted())
	{
		String res;

		if (httplib::Result &result = forecast_request->get())
		{
			// Weather data is extracted from JSON response
			JsonPtr json = Json::create();
			json->parse(result->body.c_str());
			Log::message("response:\n%s\n", json->getFormattedSubTree().get());

			// Current weather values and units are formatted for display
			auto current = json->getChild("current");
			auto current_units = json->getChild("current_units");
			if (!current || !current_units)
			{
				Log::error("SimpleHttpRequestSample: unexpected forecast JSON structure\n");
			} else
			{
				auto fmt = [&](const char *label, const char *key)
				{
					auto val = current->getChild(key);
					auto unit = current_units->getChild(key);
					if (val && unit)
						res += String(label) + String::ftoa(toFloat(val->getNumber()), 1) + unit->getString() + "\n";
				};
				fmt("Temperature: ", "temperature_2m");
				fmt("Humidity: ", "relative_humidity_2m");
				fmt("Precipitation: ", "precipitation");
				fmt("Wind Speed: ", "wind_speed_10m");
				fmt("Wind Direction: ", "wind_direction_10m");
			}
		} else
		{
			String error = httplib::to_string(result.error()).c_str();
			Log::error("%s\n", error.get());
		}
		// Weather info is displayed in the status area
		sample_description_window.setStatus(res);

		forecast_request.reset();
	}
}

// Background thread queue is stopped and UI is cleaned up.
void SimpleHttpRequestSample::shutdown()
{
	ThreadQueue::get().stop();
	sample_description_window.shutdown();
}

// Search interface is created with city input and results list.
void SimpleHttpRequestSample::init_gui()
{
	sample_description_window.createWindow();

	// Window is centered on screen
	sample_description_window.getWindow()->arrange();
	auto size = Gui::getCurrent()->getSize() / 2 - ivec2(sample_description_window.getWindow()->getWidth(), sample_description_window.getWindow()->getHeight()) / 2;
	sample_description_window.getWindow()->setPosition(size.x, size.y);

	auto gui = Gui::getCurrent();

	auto &parent = sample_description_window.getWindow();

	// Search group contains input field and results list
	auto w_group_search = WidgetGroupBox::create("Search", 20, 10);
	parent->addChild(w_group_search, Gui::ALIGN_EXPAND);

	w_search = WidgetEditLine::create(gui);
	w_search->setToolTip("search city by name");
	w_group_search->addChild(w_search, Gui::ALIGN_EXPAND);

	// Key press triggers geocoding API request
	w_search->getEventKeyPressed().connect(*this, [this]()
		{
		w_search->setFontColor(vec4_white);
		String str = "/v1/search?name=";
		str += w_search->getText();

		// New async request is created and queued for background execution
		// API: geocoding-api.open-meteo.com/v1/search?name=<city>
		city_request.push_back(std::make_unique<AsyncTask<httplib::Result>>([str]() -> httplib::Result {
				httplib::Client cli("geocoding-api.open-meteo.com");
				return cli.Get(str.get());
			})); });

	w_list = WidgetListBox::create(gui);
	w_group_search->addChild(w_list, Gui::ALIGN_LEFT);

	// City selection triggers forecast API request
	w_list->getEventChanged().connect(*this, [this]()
		{
			sample_description_window.setStatus("");
			if (w_list->getCurrentItem() != -1)
			{
				sample_description_window.setStatus("request...");
				// Forecast request is built using coordinates stored in item data
				// API: api.open-meteo.com/v1/forecast?latitude=X&longitude=Y&current=...
				auto request_str = String::format("/v1/forecast?%s&current=temperature_2m,relative_humidity_2m,precipitation,wind_speed_10m,wind_direction_10m", w_list->getCurrentItemData());
				forecast_request = std::make_unique<AsyncTask<httplib::Result>>([request_str]() -> httplib::Result {
					httplib::Client cli("api.open-meteo.com");
					return cli.Get(request_str.get());
				});
			}
		});

	sample_description_window.setStatus("Type city name");
}
