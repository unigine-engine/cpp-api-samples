#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineProperties.h>

// Comprehensive reference for Component System parameter declarations.
// Demonstrates all PROP_PARAM types (scalars, vectors, content references, masks),
// PROP_STRUCT/PROP_ARRAY for complex data, and advanced args-based features
// (visibility conditions, range constraints, file filters, editor hints).
// Also shows all lifecycle callbacks (init, update variants, physics, swap, shutdown).

// For more information see these documentation pages:
// [1] https://developer.unigine.com/en/docs/latest/api/library/common/logic/component_system/cpp/class.componentbase
// [2] https://developer.unigine.com/en/docs/latest/code/formats/property_format

class ComponentParameters: public Unigine::ComponentBase
{
public:
	// -------------------- Component Definition --------------------

	// COMPONENT_DEFINE(ComponentParameters, Unigine::ComponentBase);
	COMPONENT(ComponentParameters, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("Some description");

	// -------------------- Property Settings --------------------

	PROP_NAME("ComponentParameters");
	PROP_PARENT_NAME("component_parameters_parent");
	PROP_AUTOSAVE(1);

	// ==================== Component Methods ====================

	COMPONENT_INIT(init);
	COMPONENT_UPDATE_ASYNC_THREAD(update_async_thread);
	COMPONENT_UPDATE_SYNC_THREAD(update_sync_thread);
	COMPONENT_UPDATE(update);
	COMPONENT_POST_UPDATE(post_update);
	COMPONENT_UPDATE_PHYSICS(update_physics);
	COMPONENT_SWAP(swap);
	COMPONENT_SHUTDOWN(shutdown);

	// =================== Parameter Types ===================

	// -------------------- Primitive Types --------------------

	PROP_GROUP("Basic");

	PROP_PARAM(Int, int_parameter, 42);
	PROP_PARAM(Toggle, toggle_parameter, false, "Toggle Parameter");
	PROP_PARAM(Switch, switch_parameter, 0, "A,B,C,D");
	PROP_PARAM(Mask, mask_parameter, 0x0000'0040);
	PROP_PARAM(Float, float_parameter);
	PROP_PARAM(Double, double_parameter);
	PROP_PARAM(String, string_parameter);

	PROP_GROUP("Vectors");

	PROP_PARAM(Vec2, vec2_parameter, Unigine::Math::vec2(1, 2));
	PROP_PARAM(Vec3, vec3_parameter, { 1.f, 2.f, 3.f }, "Vec 3 Parameter");
	PROP_PARAM(Vec4, vec4_parameter, "Vec 4 Parameter");
	PROP_PARAM(DVec2, dvec2_parameter);
	PROP_PARAM(DVec3, dvec3_parameter);
	PROP_PARAM(DVec4, dvec4_parameter);
	PROP_PARAM(IVec2, ivec2_parameter);
	PROP_PARAM(IVec3, ivec3_parameter);
	PROP_PARAM(IVec4, ivec4_parameter);
	PROP_PARAM(Color, color_parameter, Unigine::Math::vec4_white);

	PROP_GROUP("Content");

	PROP_PARAM(File, file_parameter);
	PROP_PARAM(Property, property_parameter);
	PROP_PARAM(Material, material_parameter);
	PROP_PARAM(Node, node_parameter);
	PROP_PARAM(Curve2d, curve2d_parameter);

	// -------------------- Masks --------------------

	PROP_GROUP("Masks");

	PROP_PARAM(Mask, mask_general, 0, "General Mask");
	PROP_PARAM(Mask, mask_intersection, "intersection", 0, "Intersection Mask");
	PROP_PARAM(Mask, mask_physics_intersection, "physics_intersection", 0, "Physics Intersection Mask");
	PROP_PARAM(Mask, mask_collision, "collision", 0, "Collision Mask");
	PROP_PARAM(Mask, mask_exclusion, "exclusion", 0, "Exclusion Mask");
	PROP_PARAM(Mask, mask_viewport, "viewport", 0, "Viewport Mask");
	PROP_PARAM(Mask, mask_shadows, "shadows", 0, "Shadows Mask");
	PROP_PARAM(Mask, mask_material, "material", 0, "Material Mask");
	PROP_PARAM(Mask, mask_sound_source, "sound_source", 0, "Sound Source Mask");
	PROP_PARAM(Mask, mask_sound_reverb, "sound_reverb", 0, "Sound Reverb Mask");
	PROP_PARAM(Mask, mask_sound_occlusion, "sound_occlusion", 0, "Sound Occlusion Mask");
	PROP_PARAM(Mask, mask_navigation, "navigation", 0, "Navigation Mask");
	PROP_PARAM(Mask, mask_obstacle, "obstacle", 0, "Obstacle Mask");
	PROP_PARAM(Mask, mask_physical, "physical", 0, "Physical Mask");
	PROP_PARAM(Mask, mask_field, "field", 0, "Field Mask");

	// -------------------- Component Structs --------------------

	PROP_GROUP("Struct");

	struct StructParameter: public Unigine::ComponentStruct
	{
		// can have any parameters like a component
		PROP_PARAM(Int, int_parameter);
		PROP_PARAM(Float, float_parameter);
	};

	// struct parameter
	PROP_STRUCT(StructParameter, struct_parameter);

	// -------------------- Array Parameters --------------------

	PROP_GROUP("Arrays");

	// (primitive) array parameter
	PROP_ARRAY(Int, array_parameter);

	// struct array parameter
	PROP_ARRAY_STRUCT(StructParameter, struct_array_parameter);

	// ---------------------------------------------------------

	PROP_GROUP(0);

	// =================== Parameter Arguments ===================

	// For more information see see the ComponentParameterArgumentsReference
	// component below, as well as the corresponding documentation page [1].

	// - type (required)
	// - name (required)
	// - mask type (for Mask parameters only, optional)
	// - default value (except for Struct and Array parameters, optional for Vec*, Color, Property, Material, Node and Curve2d)
	// - items (for Switch parameters only, required)
	// - title (optional)
	// - tooltip (optional)
	// - group (optional)
	// - args (optional)

	PROP_PARAM(Int, int_param_args, /*default_value*/ 0, /*title*/ "Int Parameter", /*tooltip*/ "Tooltip", /*group*/ "Arguments", /*args*/ "");

	PROP_PARAM(Switch, switch_param_args, /*default_value*/ 0, /*in_items*/ "A,B,C", /*title*/ "Switch Parameter", /*tooltip*/ "Tooltip", /*group*/ "Arguments", /*args*/ "");

	PROP_PARAM(Mask, mask_param_general_args, /*default_value*/ 0, /*title*/ "General Mask", /*tooltip*/ "Tooltip", /*group*/ "Arguments", /*args*/ "");
	PROP_PARAM(Mask, mask_param_intersection_args, /*mask_type*/ "intersection", /*default_value*/ 0, /*title*/ "Intersection Mask", /*tooltip*/ "Tooltip", /*group*/ "Arguments", /*args*/ "");

	PROP_PARAM(Vec2, vec2_param_no_def_value, /*title*/ "Vec2 Parameter", /*tooltip*/ "Tooltip", /*group*/ "Arguments", /*args*/ "");
	PROP_PARAM(Vec2, vec2_param_with_def_value, /*default_value*/ Unigine::Math::vec2(0, 0), /*title*/ "Vec2 Parameter", /*tooltip*/ "Tooltip", /*group*/ "Arguments", /*args*/ "");

	PROP_PARAM(Property, property_param_no_def_value, /*title*/ "Property Parameter", /*tooltip*/ "Tooltip", /*group*/ "Arguments", /*args*/ "");
	PROP_PARAM(Property, property_param_with_def_value, /*default_value*/ Unigine::Properties::getProperty(0), /*title*/ "Property Parameter", /*tooltip*/ "Tooltip", /*group*/ "Arguments", /*args*/ "");

	PROP_STRUCT(StructParameter, struct_param_args, /*title*/ "Struct Parameter", /*tooltip*/ "Tooltip", /*group*/ "Arguments", /*args*/ "");
	PROP_ARRAY(Int, array_param_args, /*title*/ "Array Parameter", /*tooltip*/ "Tooltip", /*group*/ "Arguments", /*args*/ "");
	PROP_ARRAY_STRUCT(StructParameter, struct_array_param_args, /*title*/ "Struct Array Parameter", /*tooltip*/ "Tooltip", /*group*/ "Arguments", /*args*/ "");

	// =================== Advanced Features With Args ===================

	// Sections below take use of the `args` parameter argument to
	// specify some "advanced" attributes of the component parameters,
	// such as min/max range for scalar parameters, filters for content
	// parameters, whether the parameter is visible in the editor, etc.

	// For more information see the corresponding documentation page [2].

	// ----------------------------- Visibility -----------------------------

	// https://developer.unigine.com/en/docs/latest/code/formats/property_format#parameter_hidden

	PROP_PARAM(String, hidden_parameter, "", "Hidden Parameter", "This parameter won't be visible in the Editor", nullptr, "hidden=1");

	// ----------------------------- Scalars -----------------------------

	// https://developer.unigine.com/en/docs/latest/code/formats/property_format#parameter_min_max
	// https://developer.unigine.com/en/docs/latest/code/formats/property_format#parameter_flags

	PROP_PARAM(Int, int_range, 0, "Int Range", "This float parameter has a custom range [1, 4].", "Scalars", "min=1;max=4");
	PROP_PARAM(Float, float_range, 1.f, "Float Range", "This float parameter has a custom range [1, 4].", "Scalars", "min=1;max=4");
	PROP_PARAM(Double, double_range, 1.0, "Double Range", "This float parameter has a custom range [1, 4].", "Scalars", "min=1;max=4");

	PROP_PARAM(Float, scalar_range_expand, 2.f, "Scalar Range Expand", "This scalar parameter has a custom range [1, 4] which can be exceeded.", "Scalars", "flags=expand;min=1;max=4");
	PROP_PARAM(Float, scalar_range_expand_up, 2.f, "Scalar Range Expand Up", "This scalar parameter has a custom range [1, 4], the upper limit of which can be exceeded.", "Scalars", "flags=max_expand;min=1;max=4");
	PROP_PARAM(Float, scalar_range_expand_down, 2.f, "Scalar Range Expand Down", "This scalar parameter has a custom range [1, 4], the lower limit of which can be exceeded.", "Scalars", "flags=min_expand;min=1;max=4");

	PROP_PARAM(Float, scalar_log, 0.f, "Scalar Logarithmic", "This scalar parameter uses logarhithmic scale (base 10).", "Scalars", "flags=log10;min=0;max=100");

	// ---------------------------- Files ----------------------------

	// https://developer.unigine.com/en/docs/latest/code/formats/property_format#parameter_flags

	PROP_PARAM(File, file_asset, "", "Asset File", "This parameter accepts assets.", "Files", "flags=asset");
	PROP_PARAM(File, file_runtime, "", "Runtime File", "This parameter accepts runtimes.", "Files", "flags=runtime");
	PROP_PARAM(File, file_abspath, "", "Absolute Path File", "This parameter accepts files with absolute paths.", "Files", "flags=abspath");

	// -------------------------- Filters --------------------------

	// https://developer.unigine.com/en/docs/latest/code/formats/property_format#parameter_filter

	PROP_PARAM(File, file_filter, "", "File Filter", "This parameter accepts only .xml files", "Filters", "filter=.xml");
	PROP_PARAM(File, file_filter_set, "", "File Filter Set", "This parameter accepts .xml and .json files", "Filters", "filter=.xml|.json");
	PROP_PARAM(Material, material_filter, "Material Filter", "This parameter accepts only .basemat files", "Filters", "filter=.basemat");
	PROP_PARAM(Property, property_filter, "Property Filter", "This parameter accepts only .prop files", "Filters", "filter=.prop");

	// -------------------------- Conditionals --------------------------

	// https://developer.unigine.com/en/docs/latest/code/formats/property_format#parameter_conditions

	PROP_PARAM(Switch, independent_param, 0, "Hidden,Visible", "Independent Parameter", "The value of this parameter is used as condition for hiding/showing the parameter below.", "Conditionals");
	PROP_PARAM(String, conditional_param, "I am visible!", "Conditional Parameter", "This parameter is visible when \"Condition Parameter\" above is set to \"Visible\".", "Conditionals", "independent_param=1");

	PROP_PARAM(Int, int_param, 0, "Int Parameter", "The value of this parameter is used as condition for hiding/showing the parameter below.", "Conditionals");
	PROP_PARAM(Toggle, toggle_param, 0, "Toggle Parameter", "The value of this parameter is used as condition for hiding/showing the parameter below.", "Conditionals");
	PROP_PARAM(Switch, switch_param, 0, "A,B,C", "Switch Parameter", "The value of this parameter is used as condition for hiding/showing the parameter below.", "Conditionals");

	PROP_PARAM(String, complex_conditional_hint, "Set the integer above to \"64\", toggle to \"true\", and switch to \"C\".", "Hint", "", "Conditionals");
	PROP_PARAM(String, complex_conditional_param, "I am visible!", "Complex Conditional Parameter", "This parameter is visible when the condition described in the hint above is achieved.", "Conditionals", "int_param=64;toggle_param=1;switch_param=2");

	// -------------------------- Interfaces --------------------------

	PROP_PARAM(Property, property_internal, "Internal Property", "This parameter accepts only internal properties (that are already attached to a node in the world).", "Interfaces", "filter=.prop");
	PROP_PARAM(Property, property_interface, "Interface Property", "This parameter accepts any component that implements the specified interface. ", "Interfaces", "filter=.prop");

private:
	void init();
	void update_async_thread();
	void update_sync_thread();
	void update();
	void post_update();
	void update_physics();
	void swap();
	void shutdown();
};

// Quick reference for PROP_PARAM macro argument ordering.
// Each parameter type has its own signature with optional title/tooltip/group/args.
// Useful for copy-paste when defining new component parameters.
class ComponentParameterArgumentsReference: public Unigine::ComponentBase
{
	COMPONENT(ComponentParameterArgumentsReference, Unigine::ComponentBase);
	PROP_AUTOSAVE(0);

	struct StructParameter: public Unigine::ComponentStruct
	{
		// can have any parameters like a component
		PROP_PARAM(Int, int_parameter);
		PROP_PARAM(Float, float_parameter);
	};

	// ============= Component Parameter Arguments =============

	// -------------------------- Basic --------------------------

	// Type, const char *name, Type default_value, const char *title = nullptr, const char *tooltip = nullptr, const char *group = nullptr, const char *args = nullptr
	// ... same for Int, Toggle, Float, Double, String

	PROP_PARAM(Int, int_param, 0, "Int Parameter", "tooltip", "group", "args");

	// -------------------------- Vectors --------------------------

	// Type, const char *name, const char *title = nullptr, const char *tooltip = nullptr, const char *group = nullptr, const char *args = nullptr
	// Type, const char *name, const VEC &default_value, const char *title = nullptr, const char *tooltip = nullptr, const char *group = nullptr, const char *args = nullptr
	//  ... same for Vec2, Vec3, Vec4, DVec2, DVec3, DVec4, IVec2, IVec3, IVec4, Color

	PROP_PARAM(Vec2, vec2_param_no_def_value, "Vec2 Parameter", "tooltip", "group", "args");
	PROP_PARAM(Vec2, vec2_param_with_def_value, Unigine::Math::vec2(0, 0), "Vec2 Parameter", "tooltip", "group", "args");

	// ------------------------ Structs and Arrays ------------------------

	// Type, const char *name, const char *title = nullptr, const char *tooltip = nullptr, const char *group = nullptr, const char *args = nullptr

	PROP_STRUCT(StructParameter, struct_param, "Struct Parameter", "tooltip", "group", "args");
	PROP_ARRAY(Int, array_param, "Array Parameter", "tooltip", "group", "args");
	PROP_ARRAY_STRUCT(StructParameter, struct_array_param, "Struct Array Parameter", "tooltip", "group", "args");

	// ------------------------- Switch -------------------------

	// Type=Switch, const char *name, int default_value = 0, const char *in_items = nullptr, const char *title = nullptr, const char *tooltip = nullptr, const char *group = nullptr, const char *args = nullptr

	PROP_PARAM(Switch, switch_param, 0, "Item A,Item B,Item C", "Switch Parameter", "tooltip", "group", "args");

	// ------------------------- Masks -------------------------

	// Type=Mask, const char *name, int default_value = 0, const char *title = nullptr, const char *tooltip = nullptr, const char *group = nullptr, const char *args = nullptr

	PROP_PARAM(Mask, mask_param_general, 0, "General Mask", "tooltip", "group", "args");

	// Type=Mask, const char *name, const char *mask_type, int default_value = 0, const char *title = nullptr, const char *tooltip = nullptr, const char *group = nullptr, const char *args = nullptr
	// mask_type: intersection, physics_intersection, collision, exclusion, viewport, shadows, material, sound_source, sound_reverb, sound_occlusion, navigation, obstacle, physical, field

	PROP_PARAM(Mask, mask_param_intersection, "intersection", 0, "Intersection Mask", "tooltip", "group", "args");

	// --------------------- Content ---------------------

	// Type=File, const char *name, const char *default_value, const char *title = nullptr, const char *tooltip = nullptr, const char *group = nullptr, const char *args = nullptr

	PROP_PARAM(File, file_param, "", "File Parameter", "tooltip", "group", "args");

	// Type, const char *name, const char *title = nullptr, const char *tooltip = nullptr, const char *group = nullptr, const char *args = nullptr
	// Type, const char *name, const TypePtr &default_value, const char *title = nullptr, const char *tooltip = nullptr, const char *group = nullptr, const char *args = nullptr
	// ... same for Property, Material, Node, Curve2d

	PROP_PARAM(Property, property_param_no_def_value, "Property Parameter", "tooltip", "group", "args");
	PROP_PARAM(Property, property_param_with_def_value, Unigine::Properties::getProperty(0), "Property Parameter", "tooltip", "group", "args");

	// ------- Externally Specified Title, Tooltip or Group  -------

	PROP_TITLE("Title");
	PROP_TOOLTIP("Tooltip");
	PROP_PARAM(String, some_parameter); // this parameter will have title "Title" and tooltip "Tooltip"

	PROP_GROUP("Group");
	PROP_PARAM(Int, group_member_a); // this parameter will be a member of group "Group"
	PROP_PARAM(Float, group_member_b); // this parameter will be a member of group "Group"
	PROP_GROUP(0);
};
