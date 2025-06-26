#pragma once
#include <UnigineComponentSystem.h>

class Flame : public Unigine::ComponentBase {
	COMPONENT_DEFINE(Flame, Unigine::ComponentBase);
	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update);

	PROP_PARAM(Node, emitter_node);

public:

	void setBurn(bool burn);
	UNIGINE_INLINE bool getBurn() { return burn; }

private:
	void init();
	void update();

	Unigine::ObjectParticlesPtr emitter;
	bool burn = true;
};



