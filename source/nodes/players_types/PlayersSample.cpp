// Demonstrates the four player types and their unique behaviors:
// - Dummy: static camera with no controls, useful for cutscenes
// - Persecutor: third-person camera following a target node
// - Spectator: free-flying noclip camera with momentum
// - Actor: character controller with physics-based movement and jumping

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
		// Find a scene object to use as camera target for Persecutor
		NodePtr target = World::getNodeByName("material_ball");

		// Create fallback target if none found in scene
		if (!target)
			target = NodeDummy::create();

		// Create PlayerDummy: fixed camera with no built-in controls
		{
			dummy = PlayerDummy::create();

			float z_near = .25f;
			float z_far = 1000.f;
			float fov = 60.f;

			// Perspective projection is standard for 3D scenes
			dummy->setProjectionMode(Camera::PROJECTION_MODE_PERSPECTIVE);

			if (true) // Configure projection via individual parameters
			{
				dummy->setZNear(z_near);
				dummy->setZFar(z_far);
				dummy->setFov(fov);
			}

			else // Alternative: set projection matrix directly
			{
				// perspective() builds a standard frustum matrix
				mat4 projection_matrix = perspective(fov, 1.f, z_near, z_far);
				dummy->setProjection(projection_matrix);
			}

			// Position camera looking at target from a 3.5m radius
			dummy->setWorldTransform(setTo(rotate(Vec3_up, 0.f * 360.f) * Vec3(3.5f, 0.f, 1.5f), target->getWorldPosition(), vec3_up));
		}

		// Create PlayerPersecutor: third-person camera orbiting a target
		{
			persecutor = PlayerPersecutor::create();

			// Vertical FOV mode is more common for games
			persecutor->setFovMode(Camera::FOV_MODE_VERTICAL);
			persecutor->setFov(60.f);

			// Enable collision to prevent camera from clipping through geometry
			persecutor->setCollision(true);
			persecutor->setCollisionMask(~0);
			persecutor->setCollisionRadius(.5f);

			// Fixed=false allows camera distance to change dynamically
			persecutor->setFixed(false);
			persecutor->setMaxDistance(8.f);
			persecutor->setMinDistance(3.f);
			// Anchor offsets the look-at point from target origin (e.g., head height)
			persecutor->setAnchor(vec3(0.f, 0.f, 0.5f));

			// Set the node this camera will follow
			persecutor->setTarget(target);

			// Disable controls until this player becomes active
			persecutor->setControlled(false);

			// Position at 90 degrees around target
			persecutor->setWorldTransform(setTo(rotate(Vec3_up, .25f * 360.f) * Vec3(3.5f, 0.f, 1.5f), target->getWorldPosition(), vec3_up));
		}

		// Create PlayerSpectator: free-flying camera with momentum (noclip mode)
		{
			spectator = PlayerSpectator::create();

			spectator->setFovMode(Camera::FOV_MODE_VERTICAL);
			spectator->setFov(80.f);

			// Movement physics: acceleration creates gradual speed-up
			spectator->setAcceleration(5.f);
			spectator->setMaxVelocity(10.f);
			// Damping causes gradual slow-down when no input
			spectator->setDamping(.9f);

			// Collision prevents flying through walls
			spectator->setCollision(true);
			spectator->setCollisionMask(~0);
			spectator->setCollisionRadius(0.5f);

			spectator->setControlled(false);

			// Position at 180 degrees around target
			spectator->setWorldTransform(setTo(rotate(Vec3_up, .5f * 360.f) * Vec3(3.5f, 0.f, 1.5f), target->getWorldPosition(), vec3_up));
		}

		// Create PlayerActor: character controller with physics-based ground movement
		{
			actor = PlayerActor::create();

			// Capsule collision shape for character body
			actor->setCollision(true);
			actor->setCollisionMask(~0);
			actor->setCollisionRadius(0.5f);
			actor->setCollisionHeight(1.f);

			// Enable physical interactions with the world
			actor->setPhysical(true);
			actor->setPhysicalMask(~0);
			actor->setPhysicalMass(1.f);

			actor->setMaxVelocity(5.f);

			// Jump height in meters
			actor->setJumping(1.5f);

			actor->setControlled(false);

			// Position at 270 degrees around target
			actor->setWorldTransform(setTo(rotate(Vec3_up, .75f * 360.f) * Vec3(3.5f, 0.f, 1.5f), target->getWorldPosition(), vec3_up));
		}

		// Start with Actor as the active player
		actor->setMainPlayer(true);

		visualizer_enabled = Visualizer::isEnabled();
		Visualizer::setEnabled(true);
		gui.init(this);

		// Grab mouse for camera control
		mouse_handle = Input::getMouseHandle();
		Input::setMouseHandle(Input::MOUSE_HANDLE::MOUSE_HANDLE_GRAB);
	}

	void update()
	{
		// Render debug visualization for all non-active players
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
		// Restore original mouse handling mode
		Input::setMouseHandle(mouse_handle);
	}

	void visualize_player(PlayerPtr player)
	{
		// Skip visualization for current player (would obscure view)
		if (!player || Game::getPlayer() == player)
			return;

		vec4 label_color = vec4_white;
		vec4 frustum_color = vec4_white;
		vec4 collision_shape_color = visualize_collision_shapes ? vec4(.97f, .9f, .356f, 1.f) : vec4_zero;
		vec4 persecutor_target_color = visualize_persecutor_target ? vec4(vec3(91, 221, 247) / 255.f, 1.f) : vec4_zero;

		// Inverse modelview matrix = camera world transform
		Mat4 imodelview = player->getCamera()->getIModelview();
		Vec3 camera_position = imodelview.getTranslate();

		// getAspectCorrectedProjection() applies viewport aspect ratio
		// (raw projection matrix has aspect=1.0)
		Visualizer::renderFrustum(player->getAspectCorrectedProjection(), imodelview, frustum_color);

		{
			// Draw local coordinate axes at camera position
			float arrow_size = .6f;
			float arrow_end_size = .15f;
			float arrow_opacity = .8f;

			// Extract basis vectors from transform matrix
			Vec3 forward = imodelview.getAxisY();
			Vec3 right = imodelview.getAxisX();
			Vec3 up = imodelview.getAxisZ();

			Visualizer::renderVector(camera_position, camera_position + forward * arrow_size, vec4_green * vec4(vec3(1.f), arrow_opacity), arrow_end_size);
			Visualizer::renderVector(camera_position, camera_position + right * arrow_size, vec4_red * vec4(vec3(1.f), arrow_opacity), arrow_end_size);
			Visualizer::renderVector(camera_position, camera_position + up * arrow_size, vec4_blue * vec4(vec3(1.f), arrow_opacity), arrow_end_size);
		}

		// Type-specific visualization
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

				// Visualize collision sphere around camera
				if (persecutor->getCollision())
				{
					Visualizer::renderSphere(
						persecutor->getCollisionRadius(),
						persecutor->getWorldTransform(),
						collision_shape_color
					);
				}

				// Highlight the target node being followed
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

				// Actor uses capsule collision (cylinder with hemispherical caps)
				if (actor->getCollision())
				{
					Visualizer::renderCapsule(
						actor->getCollisionRadius(),
						actor->getCollisionHeight(),
						{
							// Position capsule above ground (center of capsule body)
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
				// Player selection buttons
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
				// Trigger click to select Actor as default
				actor_btn->runEventClicked(0);
				hbox->addChild(actor_btn);

				vbox->addChild(hbox);
			}

			// Camera parameter sliders (apply to currently active player)
			fov_slider = sample_description_window.addFloatParameter("FOV", "", 60.f, 10.f, 120.f, [](float value) { auto player = Game::getPlayer(); if (player) player->setFov(value); });
			z_far_slider = sample_description_window.addFloatParameter("Far Z", "", 1000.f, 5.f, 1000.f, [](float value) { auto player = Game::getPlayer(); if (player) player->setZFar(value); });
			z_near_slider = sample_description_window.addFloatParameter("Near Z", "", 0.25f, 0.01f, 5.f, [](float value) { auto player = Game::getPlayer(); if (player) player->setZNear(value); });
		}

		void update()
		{
			// Sync slider values with current player's camera settings
			auto player = Game::getPlayer();

			if (player)
			{
				fov_slider->setValue(int(player->getFov()) * 100);
				z_far_slider->setValue(int(player->getZFar()) * 100);
				z_near_slider->setValue(int(player->getZNear()) * 100);
			}
		}

		void shutdown() { sample_description_window.shutdown(); }

		void on_player_button_clicked(const WidgetPtr &widget, int mouse)
		{
			// Disable all button events during state change to prevent recursion
			MUTE_EVENT(dummy_btn->getEventClicked());
			MUTE_EVENT(persecutor_btn->getEventClicked());
			MUTE_EVENT(spectator_btn->getEventClicked());
			MUTE_EVENT(actor_btn->getEventClicked());

			// Untoggle all buttons
			dummy_btn->setToggled(false);
			persecutor_btn->setToggled(false);
			spectator_btn->setToggled(false);
			actor_btn->setToggled(false);

			// Disable controls on all players
			sample->persecutor->setControlled(false);
			sample->spectator->setControlled(false);
			sample->actor->setControlled(false);

			// Activate the selected player
			if (widget == dummy_btn)
			{
				// Dummy has no controls - camera stays fixed
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

	// Fixed camera with no controls
	PlayerDummyPtr dummy;
	// Third-person camera following a target
	PlayerPersecutorPtr persecutor;
	// Free-flying camera with momentum
	PlayerSpectatorPtr spectator;
	// Character controller with physics
	PlayerActorPtr actor;

	bool visualizer_enabled = false;
	bool visualize_collision_shapes = true;
	bool visualize_persecutor_target = true;

	SampleGui gui;
	// Saved mouse mode for restoration
	Input::MOUSE_HANDLE mouse_handle;
};

REGISTER_COMPONENT(PlayersSample);
