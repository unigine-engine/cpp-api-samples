#include <UnigineComponentSystem.h>
#include <UnigineVisualizer.h>
#include <UniginePlayers.h>
#include <UnigineGame.h>

#include "../../menu_ui/SampleDescriptionWindow.h"
#include "../../utils/Utils.h"

using namespace Unigine;
using namespace Math;

class PlayersSample : public ComponentBase
{
public:
	COMPONENT_DEFINE(PlayersSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init()
	{
		// get the "material_ball" node from the world to use as a target for persecutor player
		NodePtr target = World::getNodeByName("material_ball");

		if (!target)
			target = NodeDummy::create();

		// create dummy player
		{
			dummy = PlayerDummy::create();

			float z_near = .25f;
			float z_far = 1000.f;
			float fov = 60.f;

			dummy->setProjectionMode(Camera::PROJECTION_MODE_PERSPECTIVE);

			if (true) // configure camera projection parameters
			{
				dummy->setZNear(z_near);
				dummy->setZFar(z_far);
				dummy->setFov(fov);
			}

			else // alternatively you can set up the projection matrix manually
			{
				mat4 projection_matrix = perspective(fov, 1.f, z_near, z_far);
				dummy->setProjection(projection_matrix);
			}

			dummy->setWorldTransform(setTo(rotate(Vec3_up, 0.f * 360.f) * Vec3(3.5f, 0.f, 1.5f), target->getWorldPosition(), vec3_up));
		}

		// create persecutor player
		{
			persecutor = PlayerPersecutor::create();

			persecutor->setFovMode(Camera::FOV_MODE_VERTICAL);
			persecutor->setFov(60.f);

			persecutor->setCollision(true);
			persecutor->setCollisionMask(~0);
			persecutor->setCollisionRadius(.5f);

			persecutor->setFixed(false);
			persecutor->setMaxDistance(8.f);
			persecutor->setMinDistance(3.f);
			persecutor->setAnchor(vec3(0.f, 0.f, 0.5f));

			persecutor->setTarget(target);

			persecutor->setControlled(false);

			persecutor->setWorldTransform(setTo(rotate(Vec3_up, .25f * 360.f) * Vec3(3.5f, 0.f, 1.5f), target->getWorldPosition(), vec3_up));
		}

		// create spectator player
		{
			spectator = PlayerSpectator::create();

			spectator->setFovMode(Camera::FOV_MODE_VERTICAL);
			spectator->setFov(80.f);

			spectator->setAcceleration(5.f);
			spectator->setMaxVelocity(10.f);
			spectator->setDamping(.9f);

			spectator->setCollision(true);
			spectator->setCollisionMask(~0);
			spectator->setCollisionRadius(0.5f);

			spectator->setControlled(false);

			spectator->setWorldTransform(setTo(rotate(Vec3_up, .5f * 360.f) * Vec3(3.5f, 0.f, 1.5f), target->getWorldPosition(), vec3_up));
		}

		// create actor player
		{
			actor = PlayerActor::create();

			actor->setCollision(true);
			actor->setCollisionMask(~0);
			actor->setCollisionRadius(0.5f);
			actor->setCollisionHeight(1.f);

			actor->setPhysical(true);
			actor->setPhysicalMask(~0);
			actor->setPhysicalMass(1.f);

			actor->setMaxVelocity(5.f);

			actor->setJumping(1.5f);

			actor->setControlled(false);

			actor->setWorldTransform(setTo(rotate(Vec3_up, .75f * 360.f) * Vec3(3.5f, 0.f, 1.5f), target->getWorldPosition(), vec3_up));
		}

		actor->setMainPlayer(true);

		visualizer_enabled = Visualizer::isEnabled();
		Visualizer::setEnabled(true);
		gui.init(this);
		mouse_handle = Input::getMouseHandle();
		Input::setMouseHandle(Input::MOUSE_HANDLE::MOUSE_HANDLE_GRAB);
	}

	void update()
	{
		visualize_player(dummy);
		visualize_player(persecutor);
		visualize_player(spectator);
		visualize_player(actor);

		gui.update();
	}

	void shutdown()
	{
		gui.shutdown();
		Visualizer::setEnabled(visualizer_enabled);
		Input::setMouseHandle(mouse_handle);
	}

	void visualize_player(PlayerPtr player)
	{
		// render the Player's view frustum and its physical body (if it has one)

		if (!player || Game::getPlayer() == player)
			return;

		vec4 label_color = vec4_white;
		vec4 frustum_color = vec4_white;
		vec4 collision_shape_color = visualize_collision_shapes ? vec4(.97f, .9f, .356f, 1.f) : vec4_zero;
		vec4 persecutor_target_color = visualize_persecutor_target ? vec4(vec3(91, 221, 247) / 255.f, 1.f) : vec4_zero;

		// note that the inverse of the camera's view matrix is the camera's transformation matrix
		Mat4 imodelview = player->getCamera()->getIModelview();
		Vec3 camera_position = imodelview.getTranslate();

		// remember that the aspect correction is usually done automatically on a viewport-specific basis,
		// and the aspect ratio stored in the projection matrix of the cameras is henceforth usually just 1.0,
		// so to get the "correct" projection matrix you need to use the Player::getAspectCorrectedProjection method

		Visualizer::renderFrustum(player->getAspectCorrectedProjection(), imodelview, frustum_color);

		{
			// draw the local basis vectors

			float arrow_size = .6f;
			float arrow_end_size = .15f;
			float arrow_opacity = .8f;

			Vec3 forward = imodelview.getAxisY();
			Vec3 right = imodelview.getAxisX();
			Vec3 up = imodelview.getAxisZ();

			Visualizer::renderVector(camera_position, camera_position + forward * arrow_size, vec4_green * vec4(vec3(1.f), arrow_opacity), arrow_end_size);
			Visualizer::renderVector(camera_position, camera_position + right * arrow_size, vec4_red * vec4(vec3(1.f), arrow_opacity), arrow_end_size);
			Visualizer::renderVector(camera_position, camera_position + up * arrow_size, vec4_blue * vec4(vec3(1.f), arrow_opacity), arrow_end_size);
		}

		switch (player->getType())
		{
			case Node::PLAYER_DUMMY:
			{
				Visualizer::renderMessage3D(camera_position + Vec3_up * .3f, vec3_zero, "Dummy", label_color, 1, 16);

				break;
			}

			case Node::PLAYER_PERSECUTOR:
			{
				PlayerPersecutorPtr persecutor = static_ptr_cast<PlayerPersecutor>(player);

				Visualizer::renderMessage3D(camera_position + Vec3_up * .3f, vec3_zero, "Persecutor", label_color, 1, 16);

				if (persecutor->getCollision())
				{
					Visualizer::renderSphere(
						persecutor->getCollisionRadius(),
						persecutor->getWorldTransform(),
						collision_shape_color
					);
				}

				NodePtr target = persecutor->getTarget();

				if (target)
				{
					WorldBoundBox bbox = target->getWorldBoundBox();

					Visualizer::renderMessage3D(bbox.getCenter() + Vec3_up * (bbox.getSize().y * .5f + .2f), vec3_zero, "Persecutor Target", persecutor_target_color, 1, 16);
					Visualizer::renderNodeBoundBox(target, persecutor_target_color);
				}

				break;
			}

			case Node::PLAYER_SPECTATOR:
			{
				PlayerSpectatorPtr spectator = static_ptr_cast<PlayerSpectator>(player);

				Visualizer::renderMessage3D(camera_position + Vec3_up * .3f, vec3_zero, "Spectator", label_color, 1, 16);

				if (spectator->getCollision())
				{
					Visualizer::renderSphere(
						spectator->getCollisionRadius(),
						spectator->getWorldTransform(),
						collision_shape_color
					);
				}

				break;
			}

			case Node::PLAYER_ACTOR:
			{
				PlayerActorPtr actor = static_ptr_cast<PlayerActor>(player);

				Visualizer::renderMessage3D(camera_position + Vec3_up * .3f, vec3_zero, "Actor", label_color, 1, 16);

				if (actor->getCollision())
				{
					Visualizer::renderCapsule(
						actor->getCollisionRadius(),
						actor->getCollisionHeight(),
						{
							translate(
								actor->getWorldTransform().getAxisZ() * (
									actor->getCollisionRadius() +
									actor->getCollisionHeight() * .5f
								)
							) *
							actor->getWorldTransform(),
						},
						collision_shape_color
					);
				}

				break;
			}

			default: break;
		}
	}

	// ========================================================================================

	struct SampleGui : public EventConnections
	{
		void init(PlayersSample *sample)
		{
			this->sample = sample;

			sample_description_window.createWindow();

			auto parameters = sample_description_window.getParameterGroupBox();
			auto vbox = WidgetVBox::create();
			parameters->addChild(vbox);

			vbox->addChild(WidgetSpacer::create());

			{
				auto hbox = WidgetHBox::create();

				dummy_btn = WidgetButton::create("Dummy");
				dummy_btn->getEventClicked().connect(this, &SampleGui::on_player_button_clicked);
				dummy_btn->setToggleable(true);
				hbox->addChild(dummy_btn);

				persecutor_btn = WidgetButton::create("Persecutor");
				persecutor_btn->getEventClicked().connect(this, &SampleGui::on_player_button_clicked);
				persecutor_btn->setToggleable(true);
				hbox->addChild(persecutor_btn);

				spectator_btn = WidgetButton::create("Spectator");
				spectator_btn->getEventClicked().connect(this, &SampleGui::on_player_button_clicked);
				spectator_btn->setToggleable(true);
				hbox->addChild(spectator_btn);

				actor_btn = WidgetButton::create("Actor");
				actor_btn->getEventClicked().connect(this, &SampleGui::on_player_button_clicked);
				actor_btn->setToggleable(true);
				actor_btn->runEventClicked(0);
				hbox->addChild(actor_btn);

				vbox->addChild(hbox);
			}

			fov_slider = sample_description_window.addFloatParameter("FOV", "", 60.f, 10.f, 120.f, [](float value) { auto player = Game::getPlayer(); if (player) player->setFov(value); });
			z_far_slider = sample_description_window.addFloatParameter("Far Z", "", 1000.f, 5.f, 1000.f, [](float value) { auto player = Game::getPlayer(); if (player) player->setZFar(value); });
			z_near_slider = sample_description_window.addFloatParameter("Near Z", "", 0.25f, 0.01f, 5.f, [](float value) { auto player = Game::getPlayer(); if (player) player->setZNear(value); });
		}

		void update()
		{
			auto player = Game::getPlayer();

			if (player)
			{
				fov_slider->setValue(player->getFov() * 100);
				z_far_slider->setValue(player->getZFar() * 100);
				z_near_slider->setValue(player->getZNear() * 100);
			}
		}

		void shutdown() { sample_description_window.shutdown(); }

		void on_player_button_clicked(const WidgetPtr &widget, int mouse)
		{
			MUTE_EVENT(dummy_btn->getEventClicked());
			MUTE_EVENT(persecutor_btn->getEventClicked());
			MUTE_EVENT(spectator_btn->getEventClicked());
			MUTE_EVENT(actor_btn->getEventClicked());

			dummy_btn->setToggled(false);
			persecutor_btn->setToggled(false);
			spectator_btn->setToggled(false);
			actor_btn->setToggled(false);

			sample->persecutor->setControlled(false);
			sample->spectator->setControlled(false);
			sample->actor->setControlled(false);

			if (widget == dummy_btn)
			{
				Game::setPlayer(sample->dummy);
				dummy_btn->setToggled(true);
			}

			if (widget == persecutor_btn)
			{

				Game::setPlayer(sample->persecutor);
				sample->persecutor->setControlled(true);
				persecutor_btn->setToggled(true);
			}

			if (widget == spectator_btn)
			{
				Game::setPlayer(sample->spectator);
				sample->spectator->setControlled(true);
				spectator_btn->setToggled(true);
			}

			if (widget == actor_btn)
			{
				Game::setPlayer(sample->actor);
				sample->actor->setControlled(true);
				actor_btn->setToggled(true);
			}
		}

		PlayersSample *sample = nullptr;
		SampleDescriptionWindow sample_description_window;

		WidgetSliderPtr fov_slider;
		WidgetSliderPtr z_far_slider;
		WidgetSliderPtr z_near_slider;

		WidgetButtonPtr dummy_btn;
		WidgetButtonPtr persecutor_btn;
		WidgetButtonPtr spectator_btn;
		WidgetButtonPtr actor_btn;
	};

	// ========================================================================================

	PlayerDummyPtr dummy;
	PlayerPersecutorPtr persecutor;
	PlayerSpectatorPtr spectator;
	PlayerActorPtr actor;

	bool visualizer_enabled = false;
	bool visualize_collision_shapes = true;
	bool visualize_persecutor_target = true;

	SampleGui gui;
	Input::MOUSE_HANDLE mouse_handle;
};

REGISTER_COMPONENT(PlayersSample);
