#pragma once

#include <UnigineComponentSystem.h>
#include "../../menu_ui/SampleDescriptionWindow.h"
#include "UnigineCallback.h"
#include "UnigineFileSystem.h"
#include "UnigineWidgets.h"

class MountPointsSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(MountPointsSample, Unigine::ComponentBase)
	COMPONENT_INIT(init)
	COMPONENT_SHUTDOWN(shutdown)

private:
	SampleDescriptionWindow window;
	Unigine::WidgetVBoxPtr mount_points_labels = nullptr;

	Unigine::String folder_name = "external_folder";
	Unigine::String package_name = "external_package";
	Unigine::String image_name = "image.jpg";
	Unigine::String mounts_folder_name = "mounts";

	// Virtual path to the folder containing the .umounts files.
	Unigine::String virtual_path_umount;

	// Virtual path to the external image, accessed via mount points.
	Unigine::String folder_image_path;
	Unigine::String package_image_path;

	Unigine::ImagePtr image_folder = nullptr;
	Unigine::ImagePtr image_package = nullptr;

	Unigine::WidgetLabelPtr status_label = nullptr;
	Unigine::WidgetSpritePtr sprite_folder = nullptr;
	Unigine::WidgetSpritePtr sprite_package = nullptr;

	Unigine::WidgetVBoxPtr current_mounts;

	// This is a sample gist.
	bool add_mount(const char *absolute_path_external, const char *mount_path);
	bool remove_mount(const char *absolute_path_external);


	void create_mount_widget(Unigine::WidgetSpritePtr &sprite, const char *external_name, const char *type);
	void status_message(const char *msg, Unigine::Math::vec4 color = Unigine::Math::vec4_white);
	void show_mount(const char *absolute_path, int index);

	void show_sample_mounts();
	void update_images();
	void init_gui();

	void init();
	void shutdown();
};