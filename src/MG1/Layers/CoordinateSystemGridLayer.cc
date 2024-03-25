#include "CoordinateSystemGridLayer.hh"
#include "MG1/Common/InitInfo.hh"
#include "MG1/Components/GridComponent.hh"

#define GRID_THRESHOLD 30
#define GRID_SCALE     3

namespace mg1
{
  CoordinateSystemGridLayer::CoordinateSystemGridLayer(Scene* scene) : m_scene{ scene }
  {
    // create shader
    {
      auto uniform_meta_data = EspUniformMetaData::create();
      uniform_meta_data->establish_descriptor_set();
      uniform_meta_data->add_buffer_uniform(EspUniformShaderStage::ESP_VTX_STAGE, sizeof(glm::mat4));

      m_shader = ShaderSystem::acquire("Shaders/MG1/CoordinateSystemGridLayer/shader");
      m_shader->set_attachment_formats({ EspBlockFormat::ESP_FORMAT_R8G8B8A8_UNORM });
      m_shader->set_rasterizer_settings({ .m_polygon_mode = ESP_POLYGON_MODE_LINE, .m_cull_mode = ESP_CULL_MODE_NONE });
      m_shader->enable_depth_test(EspDepthBlockFormat::ESP_FORMAT_D32_SFLOAT, EspCompareOp::ESP_COMPARE_OP_LESS);
      m_shader->set_vertex_layouts({ GridInit::S_MODEL_PARAMS.get_vertex_layouts() });
      m_shader->set_worker_layout(std::move(uniform_meta_data));
      m_shader->build_worker();
    }

    create_coordinate_system_grid();
  }

  void CoordinateSystemGridLayer::update(float dt)
  {
    auto camera  = Scene::get_current_camera();
    glm::mat4 vp = camera->get_projection() * camera->get_view();

    for (auto&& [entity, grid, model] : m_scene->get_view<GridComponent, ModelComponent>())
    {
      auto& uniform_manager = model.get_uniform_manager();
      uniform_manager.update_buffer_uniform(0, 0, 0, sizeof(glm::mat4), &vp);
    }
  }

  void CoordinateSystemGridLayer::handle_event(esp::Event& event, float dt)
  {
    Event::try_handler<GuiCheckboxChangedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(CoordinateSystemGridLayer::gui_checkbox_changed_event_handler));
  }

  bool CoordinateSystemGridLayer::gui_checkbox_changed_event_handler(mg1::GuiCheckboxChangedEvent& event)
  {
    if (!(event == GuiLabel::grid_checkbox)) { return false; }
    if (event.get_value()) { push_grid(); }
    else { pop_grid(); }

    return true;
  }

  void CoordinateSystemGridLayer::create_coordinate_system_grid()
  {
    auto entity = m_scene->create_entity();

    entity->add_component<GridComponent>(entity->get_id());
    auto& cursor = entity->get_component<GridComponent>();

    auto [vertices, indices] = GridComponent::construct();
    auto model               = std::make_shared<Model>(vertices,
                                         indices,
                                         std::vector<std::shared_ptr<EspTexture>>{},
                                         CursorInit::S_MODEL_PARAMS);
    entity->add_component<ModelComponent>(model, m_shader);

    cursor.get_node()->attach_entity(entity);

    m_scene->get_root().add_child(cursor.get_node());
  }

  void CoordinateSystemGridLayer::push_grid()
  {
    for (auto&& [entity, grid] : m_scene->get_view<GridComponent>())
    {
      m_scene->get_root().add_child(grid.get_node());
    }
  }

  void CoordinateSystemGridLayer::pop_grid()
  {
    for (auto&& [entity, grid] : m_scene->get_view<GridComponent>())
    {
      m_scene->get_root().remove_child(grid.get_node());
    }
  }
} // namespace mg1