#pragma once

#include <UnigineComponentSystem.h>

#include "../../menu_ui/SampleDescriptionWindow.h"
#include "../../utils/AsyncTasks.h"
#include "../../utils/network/httplib.h"

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

	Unigine::WidgetListBoxPtr w_list;
	Unigine::WidgetEditLinePtr w_search;
	Unigine::Vector<std::unique_ptr<AsyncTask<httplib::Result>>> city_request;
	std::unique_ptr<AsyncTask<httplib::Result>> forecast_request;
};
