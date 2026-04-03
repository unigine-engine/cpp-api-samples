#pragma once
#include <UnigineEvent.h>
#include <UnigineString.h>
#include <UnigineWorld.h>
#include <UnigineEngine.h>

#define PROP_SWITCH_ENUM(NAME, ...) \
	enum NAME { __VA_ARGS__ }; \
	static const char* NAME##_STR = #__VA_ARGS__;

// utils for muting UnigineWidget callback
struct MuteEventScoped
{
	MuteEventScoped(Unigine::EventBase &in_event)
		: event(in_event)
	{
		event.setEnabled(false);
	};
	~MuteEventScoped()
	{
		event.setEnabled(true);
	};
	Unigine::EventBase &event;
};


UNIGINE_INLINE Unigine::StringStack<> joinPaths(const Unigine::VectorStack<char const *> &paths)
{
	Unigine::StringStack<> result = "";
	for (int i = 0; i < paths.size(); i += 1)
		result = Unigine::String::joinPaths(result, paths[i]);

	return result;
}

template<class ...Args>
UNIGINE_INLINE Unigine::StringStack<> joinPaths(Args... args)
{
	return joinPaths({ (const char *) args... });
}

UNIGINE_INLINE Unigine::StringStack<> getWorldRootPath()
{
	return joinPaths(Unigine::World::getPath(), "..");
}

UNIGINE_INLINE const char *getAPI()
{
	int api = Unigine::Render::getAPI();
	if (api == Unigine::Render::API_DIRECT3D12)
		return "DX12";
	if (api == Unigine::Render::API_VULKAN)
		return "Vulkan";
	return "NULL";
}

UNIGINE_INLINE Unigine::String getWindowTitle(const char *path = "")
{
	return Unigine::String::format("UNIGINE Engine %s: %s (%s)",
		Unigine::Engine::get()->getVersion(), path, getAPI());
}

