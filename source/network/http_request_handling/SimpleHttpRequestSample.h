// Weather forecast sample demonstrating asynchronous HTTP requests.
// Uses Open-Meteo geocoding and forecast APIs to search cities and display
// current weather data. Requests are executed on background threads via AsyncTask.

#pragma once

#include <UnigineComponentSystem.h>

#include "../../menu_ui/SampleDescriptionWindow.h"
#include "../../utils/AsyncTasks.h"
#include "../../utils/network/httplib.h"

// Demonstrates async HTTP requests with geocoding and weather API integration.
class SimpleHttpRequestSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SimpleHttpRequestSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	SampleDescriptionWindow sample_description_window;

private:
	void init();
	void update();
	void shutdown();

	void init_gui();

	Unigine::WidgetListBoxPtr w_list;						// City search results list
	Unigine::WidgetEditLinePtr w_search;					// City name input field
	Unigine::Vector<std::unique_ptr<AsyncTask<httplib::Result>>> city_request;	// Pending geocoding requests
	std::unique_ptr<AsyncTask<httplib::Result>> forecast_request;				// Pending forecast request
};
