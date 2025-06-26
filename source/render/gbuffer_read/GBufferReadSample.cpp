#include "GBufferReadSample.h"

#include "GBufferRead.h"

#include <UnigineGame.h>
#include <UnigineMathLibVec4.h>
#include <UnigineVisualizer.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(GBufferReadSample)

void GBufferReadSample::init()
{
	g_buffer_read_component = ComponentSystem::get()->getComponentInWorld<GBufferRead>();
	if (!g_buffer_read_component)
	{
		Log::error("GBufferReadSample::init: No gbuffer read component");
		return;
	}

	init_display(roughness_node_param, g_buffer_read_component->getRoughnessTexture());
	init_display(metallness_node_param, g_buffer_read_component->getMetalnessTexture());
	init_display(normal_node_param, g_buffer_read_component->getNormalTexture());
	init_display(albedo_node_param, g_buffer_read_component->getAlbedoTexture());
	init_display(depth_node_param, g_buffer_read_component->getDepthTexture());

	visualizer_enabled = Visualizer::isEnabled();
	Visualizer::setEnabled(true);

	sample_description_window.createWindow();
	{
		WidgetGroupBoxPtr parameters = sample_description_window.getParameterGroupBox();

		auto label = WidgetLabel::create("Render <b>G-buffer</b> from:");
		label->setFontRich(1);
		parameters->addChild(label, Gui::ALIGN_EXPAND);

		auto buttons_hbox = WidgetHBox::create();
		parameters->addChild(buttons_hbox, Gui::ALIGN_EXPAND);

		auto main_camera_button = WidgetButton::create("Main Camera");
		main_camera_button->setToggleable(true);
		auto custom_camera_button = WidgetButton::create("Custom Camera");
		custom_camera_button->setToggleable(true);

		buttons_hbox->addChild(main_camera_button, Gui::ALIGN_EXPAND);
		buttons_hbox->addChild(custom_camera_button, Gui::ALIGN_EXPAND);

		main_camera_button->getEventClicked().connect(*this, [this, main_camera_button, custom_camera_button]()
			{
				main_camera_button->getEventClicked().setEnabled(false);
				main_camera_button->setToggled(true);
				main_camera_button->getEventClicked().setEnabled(true);

				custom_camera_button->getEventClicked().setEnabled(false);
				custom_camera_button->setToggled(false);
				custom_camera_button->getEventClicked().setEnabled(true);

				if (g_buffer_read_component->getMode() != GBufferRead::Mode::RenderMainViewport)
				{
					g_buffer_read_component->setMode(GBufferRead::Mode::RenderMainViewport);
				}
			});

		custom_camera_button->getEventClicked().connect(*this, [this, main_camera_button, custom_camera_button]()
			{
				main_camera_button->getEventClicked().setEnabled(false);
				main_camera_button->setToggled(false);
				main_camera_button->getEventClicked().setEnabled(true);

				custom_camera_button->getEventClicked().setEnabled(false);
				custom_camera_button->setToggled(true);
				custom_camera_button->getEventClicked().setEnabled(true);

				if (g_buffer_read_component->getMode() != GBufferRead::Mode::RenderNode)
				{
					g_buffer_read_component->setMode(GBufferRead::Mode::RenderNode);
				}
			});

		custom_camera_button->runEventClicked(0);
	}
}

void GBufferReadSample::update()
{
	float ifps = Game::getIFps();

	WorldBoundSphere bound_sphere = node_to_render->getWorldBoundSphere();

	Vec3 camera_target = bound_sphere.center;
	float camera_distance = (float)bound_sphere.radius;
	float camera_radius = (float)bound_sphere.radius * 1.5f;

	phi += 90.0f * ifps;
	Mat4 camera_transform = translate(camera_target)
		* Mat4(rotateZ(phi) * rotateX(90.0f - THETA) * translate(0.0f, 0.0f, -camera_distance));
	if (!Render::isFlipped())
	{
		camera_transform *= Mat4(rotateZ(180.0f));
	}
	Mat4 model_view = inverse(camera_transform);
	mat4 projection = ortho(-camera_radius, camera_radius, -camera_radius, camera_radius,
		-camera_radius * 1.8f, camera_radius * 1.8f);

	if (g_buffer_read_component->getMode() == GBufferRead::Mode::RenderNode)
	{
		g_buffer_read_component->renderNode(model_view, Mat4(projection), node_to_render);

		// render frustum
		Visualizer::renderFrustum(projection, camera_transform, vec4_red, 0, false);
		Visualizer::renderVector(camera_transform.getTranslate()
				+ camera_transform.getAxisZ() * camera_radius * 2,
			camera_transform.getTranslate() - camera_transform.getAxisZ() * camera_radius / 2.f,
			vec4_red, 0.25, false, 0.f, false);
	}
}

void GBufferReadSample::shutdown()
{
	Visualizer::setEnabled(visualizer_enabled);

	sample_description_window.shutdown();
}

void GBufferReadSample::init_display(const Unigine::NodePtr &display_node,
	const Unigine::TexturePtr &texture)
{
	ObjectPtr obj = checked_ptr_cast<Object>(display_node);
	MaterialPtr display_material = obj->getMaterialInherit(0);
	display_material->setState("emission", 1);
	display_material->setParameterFloat4("albedo_color", vec4(0.f, 0.f, 0.f, 1.f));
	display_material->setTexture(display_material->findTexture("emission"), texture);
}
