#include "ObjectLayer.hh"
#include "MG1/Common/TorusInit.hh"
#include "MG1/Components/Components.hh"

namespace mg1
{
  ObjectLayer::ObjectLayer(Scene* scene) : m_scene{ scene }
  {
    // create shader
    {
      auto uniform_meta_data = EspUniformMetaData::create();
      uniform_meta_data->establish_descriptor_set();
      uniform_meta_data->add_buffer_uniform(EspUniformShaderStage::ESP_VTX_STAGE, sizeof(glm::mat4));
      uniform_meta_data->add_buffer_uniform(EspUniformShaderStage::ESP_FRAG_STAGE, sizeof(glm::vec3));

      m_shader = ShaderSystem::acquire("Shaders/MG1/ObjectLayer/shader");
      // m_shader->enable_depth_test(EspDepthBlockFormat::ESP_FORMAT_D32_SFLOAT, EspCompareOp::ESP_COMPARE_OP_LESS);
      m_shader->set_vertex_layouts({ TorusInit::S_MODEL_PARAMS.get_vertex_layouts() });
      m_shader->set_worker_layout(std::move(uniform_meta_data));
      m_shader->set_rasterizer_settings({ .m_polygon_mode = ESP_POLYGON_MODE_LINE, .m_cull_mode = ESP_CULL_MODE_NONE });
      m_shader->build_worker();
    }

    // m_all_objects[0] = std::make_shared<Object>("None");
  }

  void ObjectLayer::pre_update(float dt)
  {
    auto view = m_scene->m_registry.view<TorusComponent, ModelComponent>();
    for (auto&& [entity, torus, model] : view.each())
    {
      if (torus.get_info()->m_dirty)
      {
        auto [vertices, indices] = torus.reconstruct();
        model.get_model().set_vertex_buffer(vertices);
        model.get_model().set_index_buffer(indices, 0);
      }
    }
  }

  void ObjectLayer::update(float dt)
  {
    auto camera = Scene::get_current_camera();

    auto view = m_scene->m_registry.view<TorusComponent, ModelComponent>();
    for (auto&& [entity, torus, model] : view.each())
    {
      auto& uniform_manager = model.get_uniform_manager();
      glm::mat4 mvp         = camera->get_projection() * camera->get_view() * torus.get_node()->get_model_mat();
      uniform_manager.update_buffer_uniform(0, 0, 0, sizeof(glm::mat4), &mvp);

      glm::vec3 color =
          torus.get_info()->m_is_selected ? ObjectConstants::selected_color : ObjectConstants::default_color;
      uniform_manager.update_buffer_uniform(0, 1, 0, sizeof(glm::vec3), &color);
    }
  }

  void ObjectLayer::post_update(float dt)
  {
    static bool temp = true;
    if (temp)
    {
      create_initial_scene();
      temp = false;
    }
  }

  void ObjectLayer::handle_event(esp::Event& event, float dt)
  {
    Event::try_handler<GuiMouseStateChangedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(ObjectLayer::gui_mouse_state_changed_event_handler));
    Event::try_handler<GuiSelectableChangedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(ObjectLayer::gui_selectable_changed_event_handler));

    if (m_handle_mouse)
    {
      Event::try_handler<MouseMovedEvent>(event, ESP_BIND_EVENT_FOR_FUN(ObjectLayer::mouse_moved_event_handler, dt));
      Event::try_handler<MouseScrolledEvent>(event, ESP_BIND_EVENT_FOR_FUN(ObjectLayer::mouse_scrolled_event_handler));
    }
  }

  bool ObjectLayer::gui_mouse_state_changed_event_handler(mg1::GuiMouseStateChangedEvent& event)
  {
    m_handle_mouse = !(bool)event.get_state();
    return false;
  }

  bool ObjectLayer::mouse_moved_event_handler(esp::MouseMovedEvent& event, float dt)
  {
    auto view = m_scene->m_registry.view<TorusComponent>();
    for (auto&& [entity, torus] : view.each())
    {
      if (torus.get_info()->m_is_selected) { torus.handle_event(event, dt, m_rotation_axis); }
    }

    return true;
  }

  bool ObjectLayer::mouse_scrolled_event_handler(esp::MouseScrolledEvent& event)
  {
    auto view = m_scene->m_registry.view<TorusComponent>();
    for (auto&& [entity, torus] : view.each())
    {
      if (torus.get_info()->m_is_selected) { torus.handle_event(event); }
    }

    return true;
  }

  bool ObjectLayer::gui_selectable_changed_event_handler(GuiSelectableChangedEvent& event)
  {
    if (event.label_equals(GuiLabel::action_none)) { m_rotation_axis = RotationNone; }
    if (event.label_equals(GuiLabel::action_rot_ox)) { m_rotation_axis = RotationOX; }
    if (event.label_equals(GuiLabel::action_rot_oy)) { m_rotation_axis = RotationOY; }
    if (event.label_equals(GuiLabel::action_rot_oz)) { m_rotation_axis = RotationOZ; }

    return true;
  }

  void ObjectLayer::create_initial_scene()
  {
    {
      auto entity = m_scene->create_entity();

      entity->add_component<TorusComponent>(entity->get_id());
      auto& torus = entity->get_component<TorusComponent>();

      auto [vertices, indices] = torus.reconstruct();
      auto model               = std::make_shared<Model>(vertices,
                                           indices,
                                           std::vector<std::shared_ptr<EspTexture>>{},
                                           TorusInit::S_MODEL_PARAMS);

      entity->add_component<ModelComponent>(model, m_shader);

      torus.get_node()->attach_entity(entity);
      torus.get_node()->translate({ 2, 0, -5 });

      m_scene->get_root().add_child(torus.get_node());
    }

    {
      auto entity = m_scene->create_entity();

      entity->add_component<TorusComponent>(entity->get_id());
      auto& torus = entity->get_component<TorusComponent>();

      auto [vertices, indices] = torus.reconstruct();
      auto model               = std::make_shared<Model>(vertices,
                                           indices,
                                           std::vector<std::shared_ptr<EspTexture>>{},
                                           TorusInit::S_MODEL_PARAMS);

      entity->add_component<ModelComponent>(model, m_shader);

      torus.get_node()->attach_entity(entity);

      m_scene->get_root().add_child(torus.get_node());
    }

    {
      auto entity = m_scene->create_entity();

      entity->add_component<TorusComponent>(entity->get_id());
      auto& torus = entity->get_component<TorusComponent>();

      auto [vertices, indices] = torus.reconstruct();
      auto model               = std::make_shared<Model>(vertices,
                                           indices,
                                           std::vector<std::shared_ptr<EspTexture>>{},
                                           TorusInit::S_MODEL_PARAMS);

      entity->add_component<ModelComponent>(model, m_shader);

      torus.get_node()->attach_entity(entity);
      torus.get_node()->translate({ -2, 0, 5 });

      m_scene->get_root().add_child(torus.get_node());
    }
  }
} // namespace mg1