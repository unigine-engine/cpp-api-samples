// Demonstrates NodeExtern for creating custom node types with user-defined behavior.
// NodeExternBase provides the interface for custom nodes that integrate with the
// engine's scene graph. Custom nodes can define their own bounding boxes, visualization,
// and methods while still being manageable through the standard Node API.

#include <UnigineComponentSystem.h>
#include <UnigineMathLib.h>
#include <UnigineVisualizer.h>
#include <UnigineGame.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

using namespace Unigine;
using namespace Math;

// Custom node class inheriting from NodeExternBase
class MyNode : public NodeExternBase
{
public:
	// Unique identifier for this node type (used in serialization and factory creation)
	static constexpr int id = 1;

	// Default constructor: called when node is created programmatically
	MyNode()
	{
		Log::message("MyNode::MyNode(): called\n");
	}

	// Factory constructor: called when node is loaded from file or created via NodeExtern::create()
	// The void* parameter is the internal node handle
	MyNode(void *node): NodeExternBase(node)
	{
		Log::message("MyNode::MyNode(void*): called\n");
	}

	~MyNode() override
	{
		Log::message("MyNode::~MyNode(): called\n");
	}

	// Return the registered class ID for type identification
	int getClassID() override { return id; }

	// Define the local-space bounding box for culling and selection
	const BoundBox &getBoundBox() override { return bbox; }

	// Custom rendering for debug visualization
	void renderVisualizer() override
	{
		// Render bounding box in world space using node's transform
		Visualizer::renderBoundBox(bbox, getNode()->getWorldTransform(), color);
	}

	// Custom method: demonstrates extending node functionality
	void setColor(const vec4 &color)
	{
		this->color = color;
	}

private:
	// Unit cube bounding box centered at origin
	BoundBox bbox = BoundBox(vec3(-.5f), vec3(.5f));
	// Visualization color for the bounding box
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
		// Register the custom node class with the engine factory
		// This enables NodeExtern::create() to instantiate MyNode by ID
		NodeExternBase::addClassID<MyNode>(MyNode::id);

		if (true)
		{
			// Preferred method: create through engine factory
			// Engine manages lifetime and integrates with scene serialization
			NodeExternPtr node_extern = NodeExtern::create(MyNode::id);
			NodeExternBase *node_extern_base = node_extern->getNodeExtern();
			if (node_extern_base)
			{
				// Cast to concrete type for custom method access
				my_node = static_cast<MyNode *>(node_extern_base);
			}
		}

		else
		{
			// Alternative: direct instantiation (manual memory management required)
			my_node = new MyNode();
		}

		if (my_node)
		{
			// Access standard Node methods via getNode() (position, rotation, etc.)
			my_node->getNode()->setWorldPosition(Vec3(0.f, 0.f, 1.f));

			// Access custom MyNode methods directly
			my_node->setColor(vec4_white);
		}

		visualizer_enabled = Visualizer::isEnabled();
		Visualizer::setEnabled(true);
	}

	void update()
	{
		// Render the custom node's visualization each frame
		my_node->renderVisualizer();
	}

	void shutdown()
	{
		Visualizer::setEnabled(visualizer_enabled);
	}

	// ========================================================================================

	bool visualizer_enabled = false;
	// Pointer to custom node (owned by NodeExtern, not this component)
	MyNode *my_node = nullptr;
};

REGISTER_COMPONENT(NodeExternSample);
