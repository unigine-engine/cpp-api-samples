// Demonstrates polygon-based terrain modification and mesh generation.
// Supports mesh creation, decal placement, mask drawing, and height editing.
// Figures are defined by child node positions and triangulated for operations.

#include "LandscapePolygonsSample.h"
#include <UnigineVisualizer.h>
#include <UnigineGame.h>
#include <UnigineDecals.h>
#include <UnigineConsole.h>
#include "../../utils/Utils.h"

REGISTER_COMPONENT(LandscapePolygonsSample);

using namespace Unigine;
using namespace Math;

// Component parameters are validated and initial positions are stored.
void LandscapePolygonsSample::init()
{
	// Layer map editor node assignment is verified
	if (!layer_map_editor_node.get())
	{
		Log::warning("LandscapePolygonsSample::init 'Layer Map Editor' node is not assigned.\n");
		return;
	}

	// Figure nodes are validated and initial positions are captured
	for (int i = 0; i < figures.size(); i++)
	{
		if (!figures[i].get())
		{
			Log::warning("LandscapePolygonsSample::init invalid figures array value.\n");
			return;
		} else
		{
			// Figure position is stored for later reset
			figures_initial_positions.append(figures[i]->getWorldPosition());
			int child_number = figures[i]->getNumChildren();
			points_initial_positions.append();

			// Child point positions are stored for later reset
			for (int j = 0; j < child_number; j++)
			{
				points_initial_positions.last().append(figures[i]->getChild(j)->getWorldPosition());
			}
		}
	}

	// Manipulator component is retrieved from node
	manipulators = ComponentSystem::get()->getComponent<Manipulators>(manipulators_node.get());
	if (!manipulators)
	{
		Log::warning("LandscapePolygonsSample::init can't find manipulators component on 'Manipulators' node.\n");
		return;
	}

	// Rotation and scale axes are disabled for translation-only manipulation
	manipulators->setAxesRotation(false);
	manipulators->setAxesScale(false);

	// Clutter node is cast for vegetation invalidation
	clutter = checked_ptr_cast<ObjectMeshClutter>(clutter_node.get());
	if (!clutter_node.get())
	{
		Log::warning("LayerMapEditor::init clutter node in unspecified \n");
		return;
	}

	// Layer map is cast from node reference
	layer_map = checked_ptr_cast<LandscapeLayerMap>(layer_map_node.get());

	if (!layer_map)
	{
		Log::warning("LandscapePolygonsSample::init layer map is unspecified \n");
		return;
	}

	// Polygon editor is created with layer map and render material
	layer_map_editor = new LayerMapPolygonEditor(layer_map, mesh_material.get());
	layer_map_editor->setClutter(clutter);

	// Ortho material is verified for decal generation
	if (!ortho_material.get())
	{
		Log::warning("LandscapePolygonsSample::init ortho_material is unspecified \n");
		return;
	}

	// GUI controls are initialized
	init_gui();

	// Visualizer is enabled for triangulation debug rendering
	Visualizer::setEnabled(true);
}

// Triangulation visualization is rendered each frame.
void LandscapePolygonsSample::update()
{
	// Triangulation lines are drawn for all polygon figures
	draw_triangulation_all_figures();
}

// Landscape modifications are reverted and resources are cleaned up.
void LandscapePolygonsSample::shutdown()
{
	// All modifications and created nodes are reset
	reset();

	// Description window UI is destroyed
	description_window.shutdown();

	// Landscape mask visualization is disabled
	Render::setShowLandscapeMask(0);
}

// Polygon triangulation is computed and visualized for all figure nodes.
void LandscapePolygonsSample::draw_triangulation_all_figures()
{
	// Each figure is processed independently
	for (int i = 0; i < figures.size(); i++)
	{
		auto figure = figures[i];

		// Point arrays are allocated based on child count
		Vector<Vec3> points(figure->getNumChildren());
		Vector<vec2> mesh_points(figure->getNumChildren());
		Vector<unsigned short> indices = Vector<unsigned short>();
		double avgZ = 0.0;

		// Child positions are extracted as polygon vertices
		for (int j = 0; j < figure->getNumChildren(); j++)
		{
			points[j] = figure->getChild(j)->getWorldPosition();
			mesh_points[j] = vec2(points[j].x, points[j].y);
			avgZ += points[j].z;
		}
		avgZ /= figure->getNumChildren();

		// Polygon is triangulated using 2D projection
		triangulatePolygon(mesh_points, indices);

		// Triangle edges are rendered via visualizer
		draw_triangulation(points, indices);
	}
}

// Control panel window is created with manipulation and generation options.
void LandscapePolygonsSample::init_gui()
{
	// Main sample window is created
	description_window.createWindow();
	auto params = description_window.getParameterGroupBox();

	// Manipulator mode selection row is created
	auto figureHBox = WidgetHBox::create();
	params->addChild(figureHBox, Gui::ALIGN_LEFT);

	auto selectedFigureLabel = WidgetLabel::create("Manipulator transform mode:   ");
	figureHBox->addChild(selectedFigureLabel, Gui::ALIGN_LEFT);

	// Combo box for figure vs point transform mode is created
	WidgetComboBoxPtr manipulatorComboBox = WidgetComboBox::create();
	figureHBox->addChild(manipulatorComboBox);

	manipulatorComboBox->addItem("Figure");
	manipulatorComboBox->addItem("Point");

	// Mode change callback is connected
	manipulatorComboBox->getEventChanged().connect(*this, [this, manipulatorComboBox]()
		{
			int item = manipulatorComboBox->getCurrentItem();
			switch (item)
			{
				case 0://Figure
				{
					// Parent transform mode is enabled
					manipulators->transform_parent = true;
					manipulators->updateManipulatorTransform();
					break;
				}
				case 1://Point
				{
					// Individual point transform mode is enabled
					manipulators->transform_parent = false;
					manipulators->updateManipulatorTransform();
					break;
				}
				default:
					break;
			}
		});

	// Figure mode is selected by default
	manipulatorComboBox->setCurrentItem(0);

	// Mask visibility toggle row is created
	auto renderMaskHBox = WidgetHBox::create();
	params->addChild(renderMaskHBox, Gui::ALIGN_LEFT);

	auto showClutterMaskLabel = WidgetLabel::create("Show layer map's clutter mask:   ");
	renderMaskHBox->addChild(showClutterMaskLabel, Gui::ALIGN_LEFT);

	// Checkbox for landscape mask visualization is created
	WidgetCheckBoxPtr renderMaskCheckBox = WidgetCheckBox::create();
	renderMaskHBox->addChild(renderMaskCheckBox);

	// Mask toggle callback is connected
	renderMaskCheckBox->getEventChanged().connect(*this, [this, renderMaskCheckBox]()
		{
			if (renderMaskCheckBox->isChecked())
			{
				// Created nodes are hidden for mask visualization
				for (auto& node : created_nodes)
				{
					node->setEnabled(false);
				}
				Render::setShowLandscapeMask(1);

			} else
			{
				// Created nodes are shown again
				for (auto& node : created_nodes)
				{
					node->setEnabled(true);
				}
				Render::setShowLandscapeMask(0);
			}
		});

	// Feature buttons section is created
	auto featuresLabel = WidgetLabel::create("Generation features:");
	params->addChild(featuresLabel, Gui::ALIGN_LEFT);
	auto window = description_window.getWindow();

	// Button grid layout is created with 3 columns
	auto buttons_grid_box = WidgetGridBox::create(3, 2, 2);
	params->addChild(buttons_grid_box);

	// ObjectMeshStatic generation button is added
	auto button = WidgetButton::create("ObjectMeshStatic");
	button->getEventClicked().connect(this, &LandscapePolygonsSample::generate_mesh_button);
	buttons_grid_box->addChild(button, Gui::ALIGN_EXPAND);

	// DecalMesh generation button is added
	button = WidgetButton::create("DecalMesh");
	button->getEventClicked().connect(this, &LandscapePolygonsSample::generate_decal_mesh_button);
	buttons_grid_box->addChild(button, Gui::ALIGN_EXPAND);

	// DecalOrtho generation button is added
	button = WidgetButton::create("DecalOrtho");
	button->getEventClicked().connect(this, &LandscapePolygonsSample::generate_decal_ortho_button);
	buttons_grid_box->addChild(button, Gui::ALIGN_EXPAND);

	// Terrain mask drawing button is added
	button = WidgetButton::create("Terrain mask");
	button->getEventClicked().connect(this, &LandscapePolygonsSample::draw_terrain_mask_button);
	buttons_grid_box->addChild(button, Gui::ALIGN_EXPAND);

	// Terrain leveling button is added
	button = WidgetButton::create("Level terrain");
	button->getEventClicked().connect(this, &LandscapePolygonsSample::level_terrain_button);
	buttons_grid_box->addChild(button, Gui::ALIGN_EXPAND);

	// Terrain lowering button is added
	button = WidgetButton::create("Lower terrain");
	button->getEventClicked().connect(this, &LandscapePolygonsSample::lower_terrain_button);
	buttons_grid_box->addChild(button, Gui::ALIGN_EXPAND);

	// Reset button is added at bottom
	button = WidgetButton::create("Reset");
	button->getEventClicked().connect(this, &LandscapePolygonsSample::reset);
	params->addChild(button, Gui::ALIGN_BOTTOM);
}



// Landscape modifications are reverted and created nodes are removed.
void LandscapePolygonsSample::reset()
{
	// Async landscape modification reset is triggered
	Landscape::asyncResetModifications(layer_map->getGUID());

	// Created nodes are scheduled for deletion
	for (auto& n : created_nodes)
	{
		n.deleteLater();
	}
	created_nodes.clear();

	// Vegetation clutter is invalidated to refresh
	clutter->invalidate();

	// Figure and point positions are restored
	revert_points_positions();
}

// Mesh is rendered to texture using orthographic projection for decal generation.
void LandscapePolygonsSample::render_mesh_to_texture(Unigine::MeshPtr mesh, Unigine::TexturePtr texture, Unigine::MaterialPtr render_material, Unigine::Math::vec4 color)
{
	// Input parameters are validated
	if (!texture)
	{
		Log::error("MeshToMaskTextureSample::renderMesh: texture in nullptr \n");
		return;
	}
	if (!mesh)
	{
		Log::error("MeshToMaskTextureSample::renderMesh: mesh in nullptr \n");
		return;
	}
	if (!render_material)
	{
		Log::error("MeshToMaskTextureSample::renderMesh: material is nullptr\n");
		return;
	}

	// Mesh render object is created and loaded
	MeshRenderPtr meshRender = MeshRender::create();
	meshRender->load(mesh);

	// Orthographic projection is computed from mesh bounds
	Mat4 mesh_transform = Mat4_identity;
	double half_width = mesh->getBoundBox().getSize().x * 0.5f;
	double half_height = mesh->getBoundBox().getSize().y * 0.5f;
	mat4 projection = Math::ortho(
		-half_width, half_width,
		-half_height, half_height,
		0.01, 1000
	);

	// Top-down camera transform is computed
	Mat4 camera_transform = Math::setTo(mesh_transform.getTranslate() + Vec3_up * 10, mesh_transform.getTranslate(), vec3_forward, Math::AXIS_NZ);

	// Temporary render target is acquired
	auto renderTarget = Render::getTemporaryRenderTarget();

	// Render state is preserved and cleared
	RenderState::saveState();
	RenderState::clearStates();
	RenderState::setBlendFunc(RenderState::BLEND_NONE, RenderState::BLEND_NONE);
	RenderState::clearBuffer(RenderState::BUFFER_COLOR, vec4_black);

	// Texture is bound as color output
	renderTarget->bindColorTexture(0, texture);
	renderTarget->enable();

	// Camera matrices are configured
	Renderer::setModelview(inverse((Mat4)camera_transform));
	Renderer::setOldModelview(inverse((Mat4)camera_transform));
	Renderer::setProjection(projection);
	Renderer::setOldProjection(projection);

	// Transform matrices are computed for shader
	vec4 transforms[3];
	Mat4 local_transform = Mat4(mesh_transform);
	Mat4* transform_array = &local_transform;
	Simd::mulMat4Mat4(transforms, inverse(camera_transform), (const Mat4**)&transform_array, 1);

	// Shader parameters are configured
	ShaderPtr shader = render_material->getShaderForce("render_mesh");
	shader->setParameterArrayFloat4("s_transform", transforms, 3);
	render_material->setParameterFloat4("mesh_color", color);

	// Render pass is executed
	auto pass = render_material->getRenderPass("render_mesh");
	Renderer::setMaterial(pass, render_material);
	Renderer::setShaderParameters(pass, render_material, 0);

	// Mesh is rendered as triangles
	meshRender->render(MeshRender::MODE_TRIANGLES);

	// Render target is disabled and unbound
	renderTarget->disable();
	renderTarget->unbindAll();

	// Render state is restored and target is released
	RenderState::restoreState();
	Render::releaseTemporaryRenderTarget(renderTarget);
}

// ObjectMeshStatic objects are created from polygon figures.
void LandscapePolygonsSample::generate_mesh_button()
{
	// Each figure is converted to a mesh object
	for (int i = 0; i < figures.size(); i++)
	{
		auto figure = figures[i];
		Vector<Vec3> points;

		// Child node positions are collected as vertices
		for (int j = 0; j < figure->getNumChildren(); j++)
		{
			points.append(figure->getChild(j)->getWorldPosition());
		}

		// Bounding box is computed from vertex positions
		WorldBoundBox bb = WorldBoundBox(points.get(), points.size());

		// Triangulated mesh is generated from polygon vertices
		auto mesh = GeometryGenerator::CreatePolygonMesh(points);
		if (!mesh)
			continue;

		// Mesh object is created with dynamic procedural mode
		auto meshObj = ObjectMeshStatic::create();
		meshObj->setMeshProceduralMode(ObjectMeshStatic::PROCEDURAL_MODE_DYNAMIC, 0);
		meshObj->applyMoveMeshProceduralForce(mesh, 0);
		meshObj->setWorldPosition(bb.getCenter());

		// Intersection is enabled for raycasting
		meshObj->setIntersection(true, 0);
		meshObj->setIntersectionMask(1 << 0, 0);
		meshObj->setImmovable(false);

		// Created node is tracked for cleanup
		created_nodes.append(meshObj);
	}
}

// DecalMesh objects are created from polygon figures.
void LandscapePolygonsSample::generate_decal_mesh_button()
{
	// Each figure is converted to a mesh decal
	for (int i = 0; i < figures.size(); i++)
	{
		auto figure = figures[i];
		Vector<Vec3> points;

		// Child node positions are collected as vertices
		for (int j = 0; j < figure->getNumChildren(); j++)
		{
			points.append(figure->getChild(j)->getWorldPosition());
		}

		// Bounding box is computed from vertex positions
		WorldBoundBox bb = WorldBoundBox(points.get(), points.size());

		// Triangulated mesh is generated from polygon vertices
		auto mesh = GeometryGenerator::CreatePolygonMesh(points);
		if (!mesh)
			continue;

		// Mesh decal is created with dynamic procedural mode
		auto decal = Unigine::DecalMesh::create();
		decal->setMeshProceduralMode(ObjectMeshStatic::PROCEDURAL_MODE_DYNAMIC, 0);
		decal->applyMoveMeshProceduralForce(mesh, 0);

		// Decal is positioned above polygon center
		decal->setWorldPosition(Vec3(bb.getCenter().x, bb.getCenter().y, bb.getCenter().z + 10));
		decal->setRadius(200);

		// Base decal material is assigned
		auto mat = Materials::findManualMaterial("Unigine::decal_base");
		decal->setMaterial(mat);
		decal->setViewportMask(~0);
		decal->setSaveToWorldEnabled(true);

		// Created node is tracked for cleanup
		created_nodes.append(decal);
	}
}

// DecalOrtho objects are created with rendered polygon textures.
void LandscapePolygonsSample::generate_decal_ortho_button()
{
	// Each figure is converted to an ortho decal with rendered texture
	for (int i = 0; i < figures.size(); i++)
	{
		auto figure = figures[i];
		Vector<Vec3> points;

		// Child node positions are collected as vertices
		for (int j = 0; j < figure->getNumChildren(); j++)
		{
			points.append(figure->getChild(j)->getWorldPosition());
		}

		// Bounding box is computed from vertex positions
		WorldBoundBox bb = WorldBoundBox(points.get(), points.size());

		// Triangulated mesh is generated from polygon vertices
		auto mesh = GeometryGenerator::CreatePolygonMesh(points);
		if (!mesh)
			continue;

		// Render texture is created for decal albedo
		auto tex = Texture::create();
		tex->create2D(512, 512, Texture::FORMAT_RGBA8, Texture::FORMAT_USAGE_UNORDERED_ACCESS | Texture::FORMAT_USAGE_RENDER);
		tex->clearBuffer(vec4_zero);

		// Mesh is rendered to texture using orthographic projection
		render_mesh_to_texture(mesh, tex, mesh_material);
		ortho_decal_textures.append(tex);

		// Material instance is created with rendered texture
		auto mat = ortho_material->inherit();
		mat->setTexture("albedo", tex);

		// Ortho decal is created and positioned above polygon
		auto decal = Unigine::DecalOrtho::create();
		decal->setWorldPosition(Vec3(bb.getCenter().x, bb.getCenter().y, bb.maximum.z + 10));
		decal->setRadius(200);
		decal->setMaterial(mat);

		// Decal dimensions are set from bounding box
		decal->setWidth(bb.getSize().x);
		decal->setHeight(bb.getSize().y);
		decal->setSaveToWorldEnabled(true);

		// Created node is tracked for cleanup
		created_nodes.append(decal);
	}
}

// Terrain mask is drawn using polygon shapes.
void LandscapePolygonsSample::draw_terrain_mask_button()
{
	// Each figure is drawn onto terrain mask
	for (int i = 0; i < figures.size(); i++)
	{
		auto figure = figures[i];
		Vector<Vec3> points;

		// Child node positions are collected as vertices
		for (int j = 0; j < figure->getNumChildren(); j++)
		{
			points.append(figure->getChild(j)->getWorldPosition());
		}

		// Polygon is rendered to landscape mask layer
		layer_map_editor->generateMeshOnMask(points, Landscape::FLAGS_DATA_MASK_0);

		// Clutter refresh is scheduled
		is_clutter_mask_valid = false;
		invalidation_frame = Game::getFrame();
	}
}

// Terrain height is leveled within polygon bounds.
void LandscapePolygonsSample::level_terrain_button()
{
	// Each figure's area is leveled to minimum height
	for (int i = 0; i < figures.size(); i++)
	{
		auto figure = figures[i];
		Vector<Vec3> points;

		// Child node positions are collected as vertices
		for (int j = 0; j < figure->getNumChildren(); j++)
		{
			points.append(figure->getChild(j)->getWorldPosition());
		}

		// Bounding box is computed for height reference
		WorldBoundBox bb = WorldBoundBox(points.get(), points.size());

		// Terrain is leveled to lowest point in polygon
		layer_map_editor->levelHeightForMesh(points, bb.minimum.z);
	}
}

// Terrain height is lowered within polygon bounds.
void LandscapePolygonsSample::lower_terrain_button()
{
	// Each figure's area is lowered by fixed amount
	for (int i = 0; i < figures.size(); i++)
	{
		auto figure = figures[i];
		Vector<Vec3> points;

		// Child node positions are collected as vertices
		for (int j = 0; j < figure->getNumChildren(); j++)
		{
			points.append(figure->getChild(j)->getWorldPosition());
		}

		// Terrain is lowered by 20 units within polygon
		layer_map_editor->lowerTerrain(points, 20.0f);
	}

}

// Figure and point nodes are restored to their initial positions.
void LandscapePolygonsSample::revert_points_positions()
{
	// Each figure is restored to initial position
	for (int figure = 0; figure < figures.size(); figure++)
	{
		// Figure parent position is restored
		figures[figure]->setWorldPosition(figures_initial_positions[figure]);
		int child_number = figures.get(figure)->getNumChildren();

		// Child point positions are restored
		for (int point = 0; point < child_number; point++)
		{
			figures[figure]->getChild(point)->setWorldPosition(points_initial_positions[figure][point]);
		}
	}

	// Manipulator transform is refreshed
	manipulators->updateManipulatorTransform();
}


// Triangle edges are rendered using visualizer for debug display.
void LandscapePolygonsSample::draw_triangulation(
	const Unigine::Vector<Unigine::Math::Vec3>& points,
	const Unigine::Vector<unsigned short>& indices)
{
	// Each triangle is rendered as three edges
	for (int i = 0; i + 2 < indices.size(); i += 3)
	{
		// Vertex indices are extracted for current triangle
		const unsigned ia = indices[i + 0];
		const unsigned ib = indices[i + 1];
		const unsigned ic = indices[i + 2];

		// Vertex positions are retrieved
		const Vec3& A = points[ia];
		const Vec3& B = points[ib];
		const Vec3& C = points[ic];

		// Triangle edges are drawn in red
		Visualizer::renderLine3D(A, B, vec4_red);
		Visualizer::renderLine3D(B, C, vec4_red);
		Visualizer::renderLine3D(C, A, vec4_red);
	}
}
