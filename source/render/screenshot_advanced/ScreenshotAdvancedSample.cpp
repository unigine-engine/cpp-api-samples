#include "ScreenshotAdvancedSample.h"
#include <UnigineWindowManager.h>
#include <UnigineConsole.h>
#include <UnigineGame.h>
#include <chrono>
#include <ctime>

REGISTER_COMPONENT(ScreenshotAdvancedSample);

using namespace Unigine;

void ScreenshotAdvancedSample::init()
{
	description_window.createWindow();
	const auto& parameterGroupBox = description_window.getParameterGroupBox();
	auto savePathLabel = WidgetLabel::create(String::format("<p>Save path - \"%s\"</p>", FileSystem::getAbsolutePath(base_path.get()).get()));
	savePathLabel->setFontRich(1);
	savePathLabel->setFontWrap(1);
	parameterGroupBox->addChild(savePathLabel);

	windows.push_back(createWindowWithCamera("Window 1", Math::ivec2(700, 60), Math::Vec3(0, 5, 7), Math::vec3_forward));
	windows.push_back(createWindowWithCamera("Window 2", Math::ivec2(700, 420), Math::Vec3(7, 5, 1), Math::vec3_left));
	windows.push_back(createWindowWithCamera("Window 3", Math::ivec2(60, 420), Math::Vec3(0, 12, 7), Math::vec3_back));

	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);
}

void ScreenshotAdvancedSample::update()
{
	if (Console::isActive())
		return;

	auto focusedWindow = WindowManager::getFocusedWindow();

	if (Input::isKeyDown(Input::KEY_T))
	{
		if (!focusedWindow)
		{
			Log::error("ScreenshotMaker::update(): no focused window!\n");
			return;
		}

		Utils::ScreenshotMaker::grab(focusedWindow, generatePath("window_"), false);
	}

	if (Input::isKeyDown(Input::KEY_Y))
	{
		if (!focusedWindow)
		{
			Log::error("ScreenshotMaker::update(): no focused window!\n");
			return;
		}

		Utils::ScreenshotMaker::grab(focusedWindow, generatePath("window_gui_"), true);
	}

	if (Input::isKeyDown(Input::KEY_U))
	{
		if (!focusedWindow)
		{
			Log::error("ScreenshotMaker::update(): no focused window!\n");
			return;
		}

		CameraPtr camera = focusedWindow->getCamera();
		if (!camera && focusedWindow->isMain())
			camera = Game::getPlayer()->getCamera();

		Utils::ScreenshotMaker::grab(camera, generatePath("camera_"), Math::ivec2(1920, 1080), 30);
	}

	if (Input::isKeyDown(Input::KEY_O))
	{
		Utils::ScreenshotMaker::grab(Game::getPlayer()->getCamera(), generatePath("player_"), Math::ivec2(1920, 1080), 30);
	}

	if (Input::isKeyDown(Input::KEY_P))
	{
		EngineWindowPtr window;
		for (int i = 0; i < WindowManager::getNumWindows(); ++i)
		{
			if (WindowManager::getWindow(i)->getType() == EngineWindow::ENGINE_WINDOW_GROUP)
				window = WindowManager::getWindow(i);
		}

		if (!window)
			window = focusedWindow;

		if (!window)
		{
			Log::error("ScreenshotMaker::update(): no window!\n");
			return;
		}

		Utils::ScreenshotMaker::grab(window, generatePath("group_gui_"), true);
	}
}

void ScreenshotAdvancedSample::shutdown()
{
	Input::setMouseHandle(Input::MOUSE_HANDLE_USER);

	for (const auto& w : windows)
		w.deleteLater();

	windows.clear();

	description_window.shutdown();
}

Unigine::EngineWindowPtr ScreenshotAdvancedSample::createWindowWithCamera(const String &title, const Unigine::Math::ivec2 &windowPosition,
	const Unigine::Math::Vec3 &cameraPosition, const Math::vec3 &cameraDirection)
{
	auto window = EngineWindowViewport::create(title, 580, 300);
	window->setPosition(windowPosition);
	window->show();

	auto camera = PlayerDummy::create();
	camera->setWorldPosition(cameraPosition);
	camera->setWorldDirection(cameraDirection, Math::vec3_up);
	window->setCamera(camera->getCamera());

	auto label = WidgetLabel::create(window->getGui(), title);
	label->setFontSize(40);
	window->getGui()->addChild(label, Gui::ALIGN_OVERLAP | Gui::ALIGN_LEFT | Gui::ALIGN_TOP);

	return window;
}

Unigine::String ScreenshotAdvancedSample::generatePath(const String& prefix) const
{
	Unigine::String basePath = base_path.get();
	if (!basePath.endsWith("/"))
		basePath += '/';

	std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	char stime[256]{ 0 };
	strftime(stime, sizeof(stime), name_suffix_fmt.get(), localtime(&time));

	return String::format("%s%s%s.%s", basePath.get(), prefix.get(), stime, Utils::ScreenshotMaker::STR_FORMAT[format.get()]);
}
