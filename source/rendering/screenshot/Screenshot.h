// Captures the current frame from an engine window into a PNG file.
// Uses a flag-based approach to grab screenshots on demand during
// the render callback, saving files with unique timestamps.

#pragma once
#include <UnigineComponentSystem.h>


// Captures screenshots from an engine window on demand using flag-based triggering.
class Screenshot final : public Unigine::EventConnections
{
public:
	// Associates screenshot capture with a specific engine window
	void setWindow(const Unigine::EngineWindowPtr &window);

	// Sets flag to capture screenshot on next render
	UNIGINE_INLINE void grab() { grab_flag = true;}
private:
	// Render callback that performs actual screenshot capture
	void render();

private:
	Unigine::EngineWindowPtr window_;
	// When true, next render callback will capture and save screenshot
	bool grab_flag{false};
};
