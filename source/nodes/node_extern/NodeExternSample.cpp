#include <UnigineComponentSystem.h>
#include <UnigineMathLib.h>
#include <UnigineVisualizer.h>
#include <UnigineGame.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

using namespace Unigine;
using namespace Math;

class MyNode : public NodeExternBase
{
public:
	// unique class ID
	static constexpr int id = 1;

	MyNode()
	{
		Log::message("MyNode::MyNode(): called\n");
	}

	MyNode(void *node): NodeExternBase(node)
	{
		Log::message("MyNode::MyNode(void*): called\n");
	}

	~MyNode() override
	{
		Log::message("MyNode::~MyNode(): called\n");
	}

	int getClassID() override { return id; }

	const BoundBox &getBoundBox() override { return bbox; }

	void renderVisualizer() override
	{
		Visualizer::renderBoundBox(bbox, getNode()->getWorldTransform(), color);
	}

	// custom method
	void setColor(const vec4 &color)
	{
		this->color = color;
	}

private:
	BoundBox bbox = BoundBox(vec3(-.5f), vec3(.5f));
	vec4 color = vec4_white;
};

class NodeExternSample : public ComponentBase
{
public:
	COMPONENT_DEFINE(NodeExternSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init()
	{
		NodeExternBase::addClassID<MyNode>(MyNode::id);

		if (true)
		{
			// create a new MyNode instance
			NodeExternPtr node_extern = NodeExtern::create(MyNode::id);
			NodeExternBase *node_extern_base = node_extern->getNodeExtern();
			if (node_extern_base)
			{
				my_node = static_cast<MyNode *>(node_extern_base);
			}
		}

		else
		{
			// alternatively you can instantiate MyNode directly (you'd need to free it later!)
			my_node = new MyNode();
		}

		if (my_node)
		{
			// you can access the base Node methods via NodeExternBase::getNode
			my_node->getNode()->setWorldPosition(Vec3(0.f, 0.f, 1.f));

			// ... and access the custom MyNode methods directly
			my_node->setColor(vec4_white);
		}

		visualizer_enabled = Visualizer::isEnabled();
		Visualizer::setEnabled(true);
	}

	void update()
	{
		my_node->renderVisualizer();
	}

	void shutdown()
	{
		Visualizer::setEnabled(visualizer_enabled);
	}

	// ========================================================================================

	bool visualizer_enabled = false;
	MyNode *my_node = nullptr;
};

REGISTER_COMPONENT(NodeExternSample);
