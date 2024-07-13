#include "ObjectLayer.hh"
#include "MG1/Common/Constants.hh"
#include "MG1/Common/InitInfo.hh"
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
      m_shader->set_attachment_formats({ EspBlockFormat::ESP_FORMAT_R8G8B8A8_UNORM });
      m_shader->enable_multisampling(EspSampleCountFlag::ESP_SAMPLE_COUNT_4_BIT);
      m_shader->enable_depth_test(EspDepthBlockFormat::ESP_FORMAT_D32_SFLOAT, EspCompareOp::ESP_COMPARE_OP_LESS);
      m_shader->set_vertex_layouts({ TorusInit::S_MODEL_PARAMS.get_vertex_layouts() });
      m_shader->set_worker_layout(std::move(uniform_meta_data));
      m_shader->set_rasterizer_settings({ .m_polygon_mode = ESP_POLYGON_MODE_LINE, .m_cull_mode = ESP_CULL_MODE_NONE });
      m_shader->build_worker();
    }

    // create bezier curve shader
    {
      auto uniform_meta_data = EspUniformMetaData::create();
      uniform_meta_data->establish_descriptor_set();
      uniform_meta_data->add_buffer_uniform(EspUniformShaderStage::ESP_VTX_STAGE, sizeof(glm::mat4));
      uniform_meta_data->add_buffer_uniform(EspUniformShaderStage::ESP_FRAG_STAGE, sizeof(glm::vec3));
      uniform_meta_data->add_buffer_uniform(EspUniformShaderStage::ESP_GEOM_STAGE, sizeof(glm::mat4));

      m_bezier_curve_shader = ShaderSystem::acquire("Shaders/MG1/ObjectLayer/BezierCurve/shader");
      m_bezier_curve_shader->set_attachment_formats({ EspBlockFormat::ESP_FORMAT_R8G8B8A8_UNORM });
      m_bezier_curve_shader->enable_multisampling(EspSampleCountFlag::ESP_SAMPLE_COUNT_4_BIT);
      m_bezier_curve_shader->enable_depth_test(EspDepthBlockFormat::ESP_FORMAT_D32_SFLOAT,
                                               EspCompareOp::ESP_COMPARE_OP_LESS);
      m_bezier_curve_shader->set_vertex_layouts({ BezierCurveInit::S_MODEL_PARAMS.get_vertex_layouts() });
      m_bezier_curve_shader->set_worker_layout(std::move(uniform_meta_data));
      m_bezier_curve_shader->set_rasterizer_settings(
          { .m_polygon_mode = ESP_POLYGON_MODE_POINT, .m_cull_mode = ESP_CULL_MODE_NONE });
      m_bezier_curve_shader->set_input_assembly_settings(
          { .m_primitive_topology = ESP_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY });
      m_bezier_curve_shader->build_worker();
    }
  }

  void ObjectLayer::pre_update(float dt)
  {
    remove_or_reconstruct<TorusComponent>();
    remove_or_reconstruct<PointComponent>();
    remove_or_reconstruct<BezierCurveComponent>();

    // TODO: refactor
    for (auto&& [entity, obj] : m_scene->get_view<BezierCurveComponent>())
    {
      auto info = obj.get_info();
      for (auto& point : info->m_control_points)
      {
        if (point->selected()) { info->m_dirty = true; }
      }
    }

    pre_update_unselected_objects<TorusComponent>();
    pre_update_unselected_objects<PointComponent>();

    update_mass_centre();

    pre_update_selected_objects<TorusComponent>();
    pre_update_selected_objects<PointComponent>();
  }

  void ObjectLayer::update(float dt)
  {
    update_objects<TorusComponent>();
    update_objects<PointComponent>();
    update_objects<BezierCurveComponent>();

    // TODO: refactor
    for (auto&& [entity, obj, model] : m_scene->get_view<BezierCurveComponent, ModelComponent>())
    {
      auto& uniform_manager = model.get_uniform_manager();
      uniform_manager.update_buffer_uniform(0, 2, 0, sizeof(glm::mat4), &BERNSTEIN_BASE);
    }
  }

  void ObjectLayer::post_update(float dt)
  {
    static bool first_loop = true;
    if (first_loop)
    {
      // initial scene
      create_torus({ 2, 0, -5 });
      create_torus({ -2, 0, -5 });
      create_point({ 1.5f, 1, -1 });
      create_point({ .5f, 1, 1 });
      create_point({ -.5f, 1, -1 });
      create_point({ -1.5f, 1, 1 });
      create_point({ -2.5f, 1, 0 });
      create_point({ -3.5f, 1, 0 });
      create_point({ -4.5f, 1, 0 });
      first_loop = false;
    }
  }

  void ObjectLayer::handle_event(esp::Event& event, float dt)
  {
    Event::try_handler<GuiSelectableChangedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(ObjectLayer::gui_selectable_changed_event_handler));
    Event::try_handler<GuiButtonClickedEvent>(event,
                                              ESP_BIND_EVENT_FOR_FUN(ObjectLayer::gui_button_clicked_event_handler));
    Event::try_handler<CursorPosChangedEvent>(event,
                                              ESP_BIND_EVENT_FOR_FUN(ObjectLayer::cursor_pos_changed_event_handler));
    Event::try_handler<MouseButtonPressedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(ObjectLayer::mouse_button_pressed_event_handler));
    Event::try_handler<ObjectRemovedEvent>(event, ESP_BIND_EVENT_FOR_FUN(ObjectLayer::object_removed_event_handler));
  }

  bool ObjectLayer::gui_selectable_changed_event_handler(GuiSelectableChangedEvent& event)
  {
    if (event == GuiLabel::action_none) { m_rotation_axis = RotationNone; }
    if (event == GuiLabel::action_rot_ox) { m_rotation_axis = RotationOX; }
    if (event == GuiLabel::action_rot_oy) { m_rotation_axis = RotationOY; }
    if (event == GuiLabel::action_rot_oz) { m_rotation_axis = RotationOZ; }

    return true;
  }

  bool ObjectLayer::gui_button_clicked_event_handler(mg1::GuiButtonClickedEvent& event)
  {
    if (event == GuiLabel::create_torus_button) { create_torus(m_mouse_cursor_pos); }
    if (event == GuiLabel::create_point_button) { create_point(m_mouse_cursor_pos); }
    if (event == GuiLabel::create_bezier_curve_button) { create_bezier_curve(); }

    return true;
  }

  bool ObjectLayer::cursor_pos_changed_event_handler(CursorPosChangedEvent& event)
  {
    if (!(event == ObjectLabel::cursor_pos_changed_event && event.is_type(CursorType::Mouse))) { return false; }
    m_mouse_cursor_pos = event.get_position();

    return false;
  }

  bool ObjectLayer::mouse_button_pressed_event_handler(esp::MouseButtonPressedEvent& event)
  {
    if (event.get_button_code() != GLFW_MOUSE_BUTTON_LEFT) { return false; }

    for (auto&& [entity, point] : m_scene->get_view<PointComponent>())
    {
      point.check_if_clicked();
    }

    return false;
  }

  bool ObjectLayer::object_removed_event_handler(mg1::ObjectRemovedEvent& event)
  {
    if (!(event == ObjectLabel::object_removed_event)) { return false; }
    
    for (auto&& [entity, obj] : m_scene->get_view<BezierCurveComponent>())
    {
      obj.handle_event(event);
    }

    return false;
  }

  void ObjectLayer::create_torus(glm::vec3 position)
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
    torus.get_node()->translate(position);

    m_scene->get_root().add_child(torus.get_node());
  }

  void ObjectLayer::create_point(glm::vec3 position)
  {
    auto entity = m_scene->create_entity();

    entity->add_component<PointComponent>(entity->get_id());
    auto& point = entity->get_component<PointComponent>();

    auto [vertices, indices] = point.reconstruct();
    auto model               = std::make_shared<Model>(vertices,
                                         indices,
                                         std::vector<std::shared_ptr<EspTexture>>{},
                                         PointInit::S_MODEL_PARAMS);
    entity->add_component<ModelComponent>(model, m_shader);

    point.get_node()->attach_entity(entity);
    point.get_node()->translate(position);

    m_scene->get_root().add_child(point.get_node());
  }

  void ObjectLayer::create_bezier_curve()
  {
    std::vector<PointComponent> control_points{};
    for (auto&& [entity, point] : m_scene->get_view<PointComponent>())
    {
      if (point.get_info()->selected()) { control_points.push_back(point); }
    }
    if (control_points.empty()) { return; }

    auto entity = m_scene->create_entity();

    entity->add_component<BezierCurveComponent>(entity->get_id(), m_scene, control_points);
    auto& bezier_curve = entity->get_component<BezierCurveComponent>();

    auto [vertices, indices] = bezier_curve.reconstruct();
    auto model               = std::make_shared<Model>(vertices,
                                         indices,
                                         std::vector<std::shared_ptr<EspTexture>>{},
                                         BezierCurveInit::S_MODEL_PARAMS);
    entity->add_component<ModelComponent>(model, m_bezier_curve_shader);

    bezier_curve.get_node()->attach_entity(entity);

    m_scene->get_root().add_child(bezier_curve.get_node());
  }

  void ObjectLayer::remove_object(Node* node, ObjectInfo* info)
  {
    try_remove_node_from_selected(node);

    EspJob::done_all_jobs();
    node->get_parent()->remove_child(node);
    ObjectRemovedEvent obj_removed_event{ info };
    post_event(obj_removed_event);
    m_scene->destroy_entity(info->m_id);
  }
  void ObjectLayer::try_add_node_to_selected(Node* node)
  {
    // check if node is already selected, add if isn't
    if (!m_selected.try_add(node)) { return; }

    // update mass centre
    update_mass_centre();

    // reattach node
    Node* cursor_node;
    for (auto&& [entity, cursor] : m_scene->get_view<CursorComponent>())
    {
      if (cursor.is_type(CursorType::Object))
      {
        cursor_node = cursor.get_node();
        break;
      }
    }

    if (cursor_node)
    {
      auto parent = node->get_parent();
      parent->add_child(cursor_node);
      parent->remove_child(node);
      cursor_node->add_child(node);
    }
  }

  void ObjectLayer::try_remove_node_from_selected(Node* node)
  {
    // check if node is already selected, remove if is
    if (!m_selected.try_remove(node)) { return; }

    // reattach node
    Node* cursor_node = nullptr;
    for (auto&& [entity, cursor] : m_scene->get_view<CursorComponent>())
    {
      if (cursor.is_type(CursorType::Object))
      {
        cursor_node = cursor.get_node();
        break;
      }
    }

    if (cursor_node)
    {
      auto parent       = cursor_node;
      auto grand_parent = parent->get_parent();
      parent->remove_child(node);
      grand_parent->add_child(node);
    }
  }

  void ObjectLayer::update_mass_centre()
  {
    glm::vec3 mass_sum = { 0, 0, 0 };
    for (auto& selected : m_selected.m_nodes)
    {
      mass_sum += selected->get_translation();
    }
    glm::vec3 new_mass_centre = mass_sum / (float)m_selected.m_nodes.size();
    if (m_selected.m_mass_centre != new_mass_centre)
    {
      m_selected.m_mass_centre = new_mass_centre;
      ObjectMassCentreChangedEvent event{ !m_selected.m_nodes.empty(), m_selected.m_mass_centre };
      post_event(event);
    }
  }
} // namespace mg1
