#include <memory>

#include "CadLayer.hh"
#include "Layers/Gui/GuiLayer.hh"
#include "Layers/Objects/ObjectLayer.hh"

namespace mg1
{
  CadLayer::CadLayer()
  {
    // create scene render plan [OFF-SCREEN]
    {
      m_scene_render.m_block = EspBlock::build(EspBlockFormat::ESP_FORMAT_R8G8B8A8_UNORM,
                                               EspSampleCountFlag::ESP_SAMPLE_COUNT_4_BIT,
                                               { 0.1f, 0.1f, 0.2f });

      m_scene_render.m_depth_block =
          EspDepthBlock::build(EspDepthBlockFormat::ESP_FORMAT_D32_SFLOAT,
                               EspSampleCountFlag::ESP_SAMPLE_COUNT_4_BIT,
                               EspImageUsageFlag::ESP_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
      m_scene_render.m_plan = EspRenderPlan::create();
      m_scene_render.m_plan->add_block(std::shared_ptr{ m_scene_render.m_block });
      m_scene_render.m_plan->add_depth_block(std::shared_ptr{ m_scene_render.m_depth_block });
      m_scene_render.m_plan->build();
    }

    // create anaglyph scene render plan [OFF-SCREEN]
    {
      m_anaglyph_mode_render.m_block = EspBlock::build(EspBlockFormat::ESP_FORMAT_R8G8B8A8_UNORM,
                                                       EspSampleCountFlag::ESP_SAMPLE_COUNT_4_BIT,
                                                       { 0.1f, 0.1f, 0.2f });

      m_anaglyph_mode_render.m_depth_block =
          EspDepthBlock::build(EspDepthBlockFormat::ESP_FORMAT_D32_SFLOAT,
                               EspSampleCountFlag::ESP_SAMPLE_COUNT_4_BIT,
                               EspImageUsageFlag::ESP_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
      m_anaglyph_mode_render.m_plan = EspRenderPlan::create();
      m_anaglyph_mode_render.m_plan->add_block(std::shared_ptr{ m_anaglyph_mode_render.m_block });
      m_anaglyph_mode_render.m_plan->add_depth_block(std::shared_ptr{ m_anaglyph_mode_render.m_depth_block });
      m_anaglyph_mode_render.m_plan->build();
    }

    // create final render plan [ON-SCREEN]
    {
      m_final_render.m_plan = EspRenderPlan::create_final();
      m_final_render.m_plan->build();

      std::vector<EspVertexLayout> vertex_layouts = { VTX_LAYOUT(
          sizeof(QuadVertex),
          0,
          ESP_VERTEX_INPUT_RATE_VERTEX,
          ATTR(0, EspAttrFormat::ESP_FORMAT_R32G32_SFLOAT, offsetof(QuadVertex, m_pos)),
          ATTR(1, EspAttrFormat::ESP_FORMAT_R32G32_SFLOAT, offsetof(QuadVertex, m_tex_coord))) };

      // default shader
      {
        auto uniform_meta_data = EspUniformMetaData::create();
        uniform_meta_data->establish_descriptor_set();
        uniform_meta_data->add_texture_uniform(EspUniformShaderStage::ESP_FRAG_STAGE);

        m_final_render.m_shader = ShaderSystem::acquire("Shaders/Layers/CadLayer/shader");
        m_final_render.m_shader->set_vertex_layouts(vertex_layouts);
        m_final_render.m_shader->set_worker_layout(std::move(uniform_meta_data));
        m_final_render.m_shader->build_worker();

        m_final_render.m_uniform_manager = m_final_render.m_shader->create_uniform_manager();
        m_final_render.m_uniform_manager->load_block(0, 0, m_scene_render.m_block.get());
        m_final_render.m_uniform_manager->build();
      }

      // anaglyph shader
      {
        auto uniform_meta_data = EspUniformMetaData::create();
        uniform_meta_data->establish_descriptor_set();
        uniform_meta_data->add_texture_uniform(EspUniformShaderStage::ESP_FRAG_STAGE);
        uniform_meta_data->add_texture_uniform(EspUniformShaderStage::ESP_FRAG_STAGE);

        m_final_render.m_anaglyph_mode.m_shader = ShaderSystem::acquire("Shaders/Layers/CadLayer/AnaglyphMode/shader");
        m_final_render.m_anaglyph_mode.m_shader->set_vertex_layouts(vertex_layouts);
        m_final_render.m_anaglyph_mode.m_shader->set_worker_layout(std::move(uniform_meta_data));
        m_final_render.m_anaglyph_mode.m_shader->build_worker();

        m_final_render.m_anaglyph_mode.m_uniform_manager =
            m_final_render.m_anaglyph_mode.m_shader->create_uniform_manager();
        m_final_render.m_anaglyph_mode.m_uniform_manager->load_block(0, 0, m_scene_render.m_block.get());
        m_final_render.m_anaglyph_mode.m_uniform_manager->load_block(0, 1, m_anaglyph_mode_render.m_block.get());
        m_final_render.m_anaglyph_mode.m_uniform_manager->build();
      }

      m_final_render.m_vertex_buffer = EspVertexBuffer::create(quad.data(), sizeof(QuadVertex), quad.size());
      m_final_render.m_index_buffer  = EspIndexBuffer::create(quad_idx.data(), quad_idx.size());
    }

    // create scene
    {
      m_orbit_camera = std::make_shared<OrbitCamera>();
      m_orbit_camera->set_sensitivity(1.f / 4.f);
      m_orbit_camera->set_perspective(EspWorkOrchestrator::get_swap_chain_extent_aspect_ratio());
      m_fps_camera = std::make_shared<FpsCamera>();
      m_fps_camera->set_sensitivity(2.f);
      m_fps_camera->set_move_speed(4.f);
      m_fps_camera->set_perspective(EspWorkOrchestrator::get_swap_chain_extent_aspect_ratio());
      m_scene = Scene::create();
      m_scene->add_camera(m_orbit_camera);
      m_scene->add_camera(m_fps_camera);

      m_scene->set_current_camera(m_orbit_camera.get());
    }

    // create children layers
    {
      m_children.emplace_back(new GuiLayer());
      m_children.emplace_back(new ObjectLayer(m_scene.get()));
    }
  }

  void CadLayer::pre_update(float dt)
  {
    for (auto& child : m_children)
    {
      child->pre_update(dt);
    }
  }

  void CadLayer::update(float dt)
  {
    for (auto& child : m_children)
    {
      child->update(dt);
    }

    handle_keyboard_input(dt);

    auto camera = Scene::get_current_camera();
    if (m_anaglyph_mode.m_on)
    {
      camera->set_anaglyph_perspective(m_anaglyph_mode.m_eye_dist, m_anaglyph_mode.m_plane_dist, true);
    }
    else { camera->set_perspective(EspWorkOrchestrator::get_swap_chain_extent_aspect_ratio()); }

    // scene render plan [OFF-SCREEN]
    m_scene_render.m_plan->begin_plan();
    {
      update_camera_on_scene(camera->get_projection(), camera->get_view());
      m_scene->draw();
    }
    m_scene_render.m_plan->end_plan();
    m_scene_render.m_depth_block->clear();

    if (m_anaglyph_mode.m_on)
    {
      EspWorkOrchestrator::split_frame();

      camera->set_anaglyph_perspective(m_anaglyph_mode.m_eye_dist, m_anaglyph_mode.m_plane_dist, false);

      // anaglyph scene render plan [OFF-SCREEN]
      m_anaglyph_mode_render.m_plan->begin_plan();
      {
        update_camera_on_scene(camera->get_projection(), camera->get_view());
        m_scene->draw();
      }
      m_anaglyph_mode_render.m_plan->end_plan();
      m_anaglyph_mode_render.m_depth_block->clear();
    }

    // final render plan [ON-SCREEN]
    m_final_render.m_plan->begin_plan();
    {
      if (m_anaglyph_mode.m_on)
      {
        m_final_render.m_anaglyph_mode.m_shader->attach();
        m_final_render.m_anaglyph_mode.m_uniform_manager->attach();
      }
      else
      {
        m_final_render.m_shader->attach();
        m_final_render.m_uniform_manager->attach();
      }

      m_final_render.m_vertex_buffer->attach();
      m_final_render.m_index_buffer->attach();

      EspJob::draw_indexed(quad_idx.size());
      if (EspGui::m_use_gui) { EspGui::render(); }
    }
    m_final_render.m_plan->end_plan();
  }

  void CadLayer::post_update(float dt)
  {
    for (auto& child : m_children)
    {
      child->post_update(dt);
    }
  }

  void CadLayer::handle_event(esp::Event& event, float dt)
  {
    for (auto& child : m_children)
    {
      child->handle_event(event, dt);
    }

    Event::try_handler<MouseMovedEvent>(event, ESP_BIND_EVENT_FOR_FUN(CadLayer::mouse_moved_event_handler, dt));
    Event::try_handler<GuiSelectableChangedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(CadLayer::gui_selectable_changed_event_handler));
    Event::try_handler<GuiMouseStateChangedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(CadLayer::gui_mouse_state_changed_event_handler));
    Event::try_handler<GuiCameraTypeChangedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(CadLayer::gui_camera_type_changed_event_handler));
    Event::try_handler<GuiCheckboxChangedEvent>(event,
                                                ESP_BIND_EVENT_FOR_FUN(CadLayer::gui_checkbox_changed_event_handler));
    Event::try_handler<GuiFloatSliderChangedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(CadLayer::gui_float_slider_changed_event_handler));
  }

  bool CadLayer::mouse_moved_event_handler(MouseMovedEvent& event, float dt)
  {
    if (!m_none_object_selected || !m_none_action_selected || !m_mouse_captured) { return false; }

    if (EspInput::is_mouse_button_pressed(ESP_MOUSE_BUTTON_LEFT))
    {
      if (orbit_camera_selected()) { m_orbit_camera->rotate(event.get_dx(), event.get_dy(), dt); }
      if (fps_camera_selected()) { m_fps_camera->look_at(event.get_dx(), -event.get_dy(), dt); }
    }
    if (EspInput::is_mouse_button_pressed(ESP_MOUSE_BUTTON_RIGHT))
    {
      if (orbit_camera_selected()) { m_orbit_camera->zoom(event.get_dy(), dt); }
      if (fps_camera_selected()) { m_fps_camera->zoom(event.get_dy(), dt); }
    }

    return true;
  }

  bool CadLayer::gui_selectable_changed_event_handler(GuiSelectableChangedEvent& event)
  {
    if (event == GuiLabel::object_none) { m_none_object_selected = event.get_value(); }
    if (event == GuiLabel::action_none) { m_none_action_selected = event.get_value(); }

    return true;
  }

  bool CadLayer::gui_mouse_state_changed_event_handler(mg1::GuiMouseStateChangedEvent& event)
  {
    m_mouse_captured = !(bool)event.get_state();
    return false;
  }

  bool CadLayer::gui_camera_type_changed_event_handler(mg1::GuiCameraTypeChangedEvent& event)
  {
    if (event != GuiLabel::gui_camera_type_changed_event) { return false; }

    switch (event.get_type())
    {
    case Fps:
    {
      Scene::set_current_camera(m_fps_camera.get());
      break;
    }
    case Orbit:
    {
      Scene::set_current_camera(m_orbit_camera.get());
      break;
    }
    }

    return false;
  }

  bool CadLayer::gui_checkbox_changed_event_handler(mg1::GuiCheckboxChangedEvent& event)
  {
    if (!(event == GuiLabel::anaglyph_mode)) { return false; }
    m_anaglyph_mode.m_on = event.get_value();
    return true;
  }

  bool CadLayer::gui_float_slider_changed_event_handler(GuiFloatSliderChangedEvent& event)
  {
    if (event == GuiLabel::m_eye_distance_float_slider) { m_anaglyph_mode.m_eye_dist = event.get_value(); }
    if (event == GuiLabel::m_plane_distance_float_slider) { m_anaglyph_mode.m_plane_dist = event.get_value(); }
    return true;
  }

  void CadLayer::handle_keyboard_input(float dt)
  {
    if (fps_camera_selected())
    {
      if (EspInput::is_key_pressed(ESP_KEY_W)) { m_fps_camera->move(FpsCamera::FORWARD, dt); }
      if (EspInput::is_key_pressed(ESP_KEY_S)) { m_fps_camera->move(FpsCamera::BACKWARD, dt); }
      if (EspInput::is_key_pressed(ESP_KEY_A)) { m_fps_camera->move(FpsCamera::LEFT, dt); }
      if (EspInput::is_key_pressed(ESP_KEY_D)) { m_fps_camera->move(FpsCamera::RIGHT, dt); }
      if (EspInput::is_key_pressed(ESP_KEY_SPACE)) { m_fps_camera->move(FpsCamera::UP, dt); }
      if (EspInput::is_key_pressed(ESP_KEY_LEFT_SHIFT)) { m_fps_camera->move(FpsCamera::DOWN, dt); }
    }
  }

  void CadLayer::update_camera_on_scene(glm::mat4 projection, glm::mat4 view)
  {
    m_scene->get_root().act(
        [&projection, &view](Node* node)
        {
          auto model = node->get_entity()->try_get_component<ModelComponent>();
          if (model)
          {
            auto& uniform_manager = model->get_uniform_manager();
            glm::mat4 mvp         = projection * view * node->get_model_mat();
            uniform_manager.update_buffer_uniform(0, 0, 0, sizeof(glm::mat4), &mvp);
          }
        });
  }
} // namespace mg1