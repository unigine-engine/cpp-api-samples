#pragma once

#include <UnigineComponentSystem.h>
#include "../../menu_ui/SampleDescriptionWindow.h"
#include "UnigineCallback.h"
#include "UnigineFileSystem.h"
#include "UnigineWidgets.h"

// Demonstrates the FileSystem mount point API for accessing external resources.
// External folders and packages (.zip, .ung) can be mounted into the virtual
// file system at runtime, making their contents accessible via virtual paths.
// Mount points are persisted as .umount files and can be created/removed dynamically.
class MountPointsSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(MountPointsSample, Unigine::ComponentBase)
	COMPONENT_INIT(init)
	COMPONENT_SHUTDOWN(shutdown)

private:
	SampleDescriptionWindow window;                       // Main UI window
	Unigine::WidgetVBoxPtr mount_points_labels = nullptr; // Container for mount status labels

	// Resource names for demonstration
	Unigine::String folder_name = "external_folder";
	Unigine::String package_name = "external_package";
	Unigine::String image_name = "image.jpg";
	Unigine::String mounts_folder_name = "mounts";

	// Virtual path to the folder containing the .umounts files.
	Unigine::String virtual_path_umount;

	// Virtual path to the external image, accessed via mount points.
	Unigine::String folder_image_path;
	Unigine::String package_image_path;

	Unigine::ImagePtr image_folder = nullptr;  // Preview image from mounted folder
	Unigine::ImagePtr image_package = nullptr; // Preview image from mounted package

	// UI elements
	Unigine::WidgetLabelPtr status_label = nullptr;   // Displays operation status messages
	Unigine::WidgetSpritePtr sprite_folder = nullptr;  // Shows folder mount preview
	Unigine::WidgetSpritePtr sprite_package = nullptr; // Shows package mount preview

	Unigine::WidgetVBoxPtr current_mounts; // Container for active mount point labels

	// Manage external resource mounts
	bool add_mount(const char *absolute_path_external, const char *mount_path);
	bool remove_mount(const char *absolute_path_external);

	// UI helper methods
	void create_mount_widget(Unigine::WidgetSpritePtr &sprite, const char *external_name, const char *type);
	void status_message(const char *msg, Unigine::Math::vec4 color = Unigine::Math::vec4_white);
	void show_mount(const char *absolute_path, int index);

	void show_sample_mounts(); // Refreshes mount status display
	void update_images();      // Reloads preview images from mounts
	void init_gui();           // Creates UI controls

	void init();     // Configures paths and initializes UI
	void shutdown(); // Releases UI and disconnects events
};