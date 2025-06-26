#pragma once
#include <UnigineComponentSystem.h>


class Screenshot final : public Unigine::EventConnections
{
public:
	void setWindow(const Unigine::EngineWindowPtr &window);

	UNIGINE_INLINE void grab() { grab_flag = true;}
private:
	void render();

private:
	Unigine::EngineWindowPtr window_;
	bool grab_flag{false};
};
