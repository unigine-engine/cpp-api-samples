// Demonstrates two methods for querying water surface state: fetch (direct height
// lookup) and intersection (ray-water collision). Fetch is faster but less accurate
// for angled queries; intersection handles oblique rays with configurable precision.

#include "WaterFetchIntersection.h"

#include <UnigineVisualizer.h>

#include <functional>

REGISTER_COMPONENT(WaterFetchIntersection);

using namespace Unigine;
using namespace Math;

void WaterFetchIntersection::init()
{
	// Cast the node reference to ObjectWaterGlobal
	water = checked_ptr_cast<ObjectWaterGlobal>(water_node.get());

	Visualizer::setEnabled(true);
	init_gui();
}

void WaterFetchIntersection::shutdown()
{
	Visualizer::setEnabled(false);
	shutdown_gui();
}

void WaterFetchIntersection::init_gui()
{
	sample_description_window.createWindow();

	auto parameters = sample_description_window.getParameterGroupBox();

	// Create toggle buttons for query mode selection
	auto hbox_request_type = WidgetHBox::create();
	auto request_type_label = WidgetLabel::create("Request type: ");
	auto fetch_button = WidgetButton::create("Fetch");
	auto intersection_button = WidgetButton::create("Intersection");
	normal_cb = WidgetCheckBox::create("Show normals");

	fetch_button->setToggleable(true);
	intersection_button->setToggleable(true);

	// Default to intersection mode
	intersection_button->setToggled(true);
	fetch = false;

	hbox_request_type->addChild(request_type_label);
	hbox_request_type->addChild(fetch_button);

	hbox_request_type->addChild(intersection_button);
	hbox_request_type->addChild(normal_cb);

	parameters->addChild(hbox_request_type, Gui::ALIGN_LEFT);

	// Radio button behavior: toggling one untoggles the other
	fetch_button->getEventChanged().connect(*this, [this, fetch_button, intersection_button]() {
			fetch = fetch_button->isToggled();
			intersection_button->setToggled(!fetch);
		});

	intersection_button->getEventChanged().connect(*this, [this, fetch_button, intersection_button]() {
			fetch = !intersection_button->isToggled();
			fetch_button->setToggled(fetch);
		});

	// General parameters
	sample_description_window.addIntParameter("Number of requests", "num request", num_intersection, 1, 10000,
		[this](int v) { num_intersection = v; });

	sample_description_window.addFloatParameter("Point size", "Point size", intersect_point_size, 0.1f, 1.f,
		[this](float v) { intersect_point_size = v; });

	// Beaufort scale controls wave intensity (0 = calm, 12 = hurricane)
	sample_description_window.addFloatParameter("Beaufort", "beaufort", 0.f, 0.f, 12.f,
		[this](float v) { water->setBeaufort(v); });

	sample_description_window.addParameterSpacer();

	// Fetch-specific parameters
	// Amplitude threshold: waves below this height are ignored for fetch
	sample_description_window.addFloatParameter("Fetch Amplitude Threshold", "fetch amplitude threshold",
		water->getFetchAmplitudeThreshold(), 0.001f, 0.5f,
		[this](float v) { water->setFetchAmplitudeThreshold(v); });

	// Steepness quality: higher = more accurate but slower fetch
	sample_description_window.addIntParameter("Fetch Steepness Quality", "fetch steepness quality",
		water->getFetchSteepnessQuality(), 0, 4, [this](int v) {
			water->setFetchSteepnessQuality(ObjectWaterGlobal::STEEPNESS_QUALITY(v));
		});

	sample_description_window.addParameterSpacer();

	// Intersection-specific parameters
	sample_description_window.addFloatParameter("Intersection Amplitude Threshold",
		"intersection amplitude threshold", water->getIntersectionAmplitudeThreshold(), 0.001f,
		0.5f, [this](float v) { water->setIntersectionAmplitudeThreshold(v); });

	sample_description_window.addIntParameter("Intersection Steepness Quality",
		"intersection steepness quality", static_cast<int>(water->getIntersectionSteepnessQuality()), 0, 4,
		[this](int v) {
			water->setIntersectionSteepnessQuality(ObjectWaterGlobal::STEEPNESS_QUALITY(v));
		});

	// Precision: distance threshold for considering intersection found
	sample_description_window.addFloatParameter("Intersection Precision", "intersection precision",
		water->getIntersectionPrecision(), 0.001f, 2.0f,
		[this](float v) { water->setIntersectionPrecision(v); });

	// Ray angle for intersection tests
	sample_description_window.addFloatParameter("Intersection Angle", "intersection angle", 5.f,
		-30.f, 30.f, [this](float v) { water->setIntersectionPrecision(v); });
}

void WaterFetchIntersection::shutdown_gui()
{
	sample_description_window.shutdown();
}

void WaterFetchIntersection::update()
{
	bool normal_show = normal_cb->isChecked();
	// Create a square grid of query points
	int count = (int)Math::sqrtFast((float)num_intersection);

	// Pre-create intersection result objects (reused for efficiency)
	ObjectIntersectionPtr oi = ObjectIntersection::create();
	ObjectIntersectionNormalPtr oin = ObjectIntersectionNormal::create();

	for (int i = 0; i < count; i++)
	{
		for (int j = 0; j < count; j++)
		{
			// Grid position at water level (z=0)
			Vec3 pos = Vec3((float)i, (float)j, 0);

			if (fetch)
			{
				// Fetch mode: direct height lookup at XY position
				float v = water->fetchHeight(pos);
				pos.z += v;
				Visualizer::renderPoint3D(pos, intersect_point_size, vec4_blue);

				if (normal_show)
				{
					// Fetch surface normal at this position
					vec3 n = water->fetchNormal(pos);
					Visualizer::renderVector(pos, pos + Vec3(n), vec4_white);
				}
			}
			else
			{
				// Intersection mode: cast ray to find water surface
				// Ray direction based on intersection angle parameter
				Vec3 dir(Math::sin(intersection_angle * Consts::DEG2RAD), 0,
					Math::cos(intersection_angle * Consts::DEG2RAD));

				if (normal_show)
				{
					// Use ObjectIntersectionNormal to also get surface normal
					if (water->getIntersection(pos + dir * 100, pos - dir * 100, oin, 0))
					{
						Visualizer::renderPoint3D(oin->getPoint(), intersect_point_size,
							vec4_green);
						Visualizer::renderVector(oin->getPoint(),
							oin->getPoint() + Vec3(oin->getNormal()), vec4_white);
						// Render ray direction
						Visualizer::renderVector(oin->getPoint() + dir * 2.f, oin->getPoint(),
							vec4_blue);
					}
				}
				else
				{
					// Use ObjectIntersection (no normal, slightly faster)
					if (water->getIntersection(pos + dir * 100, pos - dir * 100, oi, 0))
					{
						Visualizer::renderPoint3D(oi->getPoint(), intersect_point_size, vec4_green);
						Visualizer::renderVector(oi->getPoint() + dir * 2.f, oi->getPoint(),
							vec4_blue);
					}
				}
			}
		}
	}
}
