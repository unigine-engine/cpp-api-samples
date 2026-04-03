#pragma once
#include <UnigineObjects.h>

class LayerMapPolygonEditor {
public:
	LayerMapPolygonEditor(Unigine::LandscapeLayerMapPtr in_layer_map, Unigine::MaterialPtr material);

	~LayerMapPolygonEditor()
	{
		event_connection.disconnectAll();
	}

	void clearMasks(int flags_data_mask);
	void generateMeshOnMask(const Unigine::Vector<Unigine::Math::Vec3>& points, int flags_data_mask);
	void levelHeightForMesh(const Unigine::Vector<Unigine::Math::Vec3>& points, double height);
	void lowerTerrain(const Unigine::Vector<Unigine::Math::Vec3>& points, float amount);
	void setClutter(Unigine::ObjectMeshClutterPtr in_clutter);
private:
	enum ACTION_TYPE
	{
		LEVEL_HEIGHT,
		LOWER_HEIGHT,
		DRAW_MASK,
		CLEAR,
	};

	struct DrawData
	{
		Unigine::MeshPtr mesh;
		Unigine::Math::vec4 draw_color = Unigine::Math::vec4_black;
		int masks;
		int dilate_radius = 0;
		int blend_radius = 0;
		int channel_masks[5] = { 0,0,0,0,0 };
		float height_value;
		float height_scale = 1;

		ACTION_TYPE action_type;
	};


	Unigine::LandscapeLayerMapPtr layer_map;
	Unigine::ObjectMeshClutterPtr clutter;
	Unigine::MaterialPtr render_material;
	Unigine::HashMap<int, DrawData> operation_draw_data; // Polygon data for texture drawing
	Unigine::EventConnections event_connection;

	const char* level_height_pass = "level_height";
	const char* lower_height_pass = "lower_height";
	const char* draw_mask_pass = "draw_mask";
	const char* render_mesh_pass = "render_mesh";
	int need_clutter_update = -1;

	void update();

	void drawCallback(const Unigine::UGUID& guid, int id, const Unigine::LandscapeTexturesPtr& buffer, const Unigine::Math::ivec2& coord, int data_mask);

	void drawMasks(const DrawData& data, const Unigine::LandscapeTexturesPtr& buffer);
	void drawLevelHeight(const DrawData& data, const Unigine::LandscapeTexturesPtr& buffer);
	void drawLowerHeight(const DrawData& data, const Unigine::LandscapeTexturesPtr& buffer);
	void clearMasks(const DrawData& data, const Unigine::LandscapeTexturesPtr& buffer);

	int convertMasksToFileMasks(int flags_data_mask);
	void convertChannelMasks(const int flags_data_mask, DrawData& out);

};
