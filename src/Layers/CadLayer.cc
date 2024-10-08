#include "CadLayer.hh"
#include "Layers/Gui/GuiLayer.hh"
#include "Layers/Objects/Cursor/CursorComponent.hh"
#include "Layers/Objects/ObjectLayer.hh"

struct QuadVertex
{
  glm::vec2 m_pos;
  glm::vec2 m_tex_coord;
};

float scene_left   = mg1::WindowConstants::scene_win_pos.x;
float scene_right  = scene_left + mg1::WindowConstants::scene_win_size.x;
float scene_top    = mg1::WindowConstants::root_win_h - mg1::WindowConstants::scene_win_pos.y;
float scene_bottom = scene_top - mg1::WindowConstants::scene_win_size.y;
float quad_left    = CLIP_SCALE(scene_left, mg1::WindowConstants::root_win_w);
float quad_right   = CLIP_SCALE(scene_right, mg1::WindowConstants::root_win_w);
float quad_top     = CLIP_SCALE(scene_top, mg1::WindowConstants::root_win_h);
float quad_bottom  = CLIP_SCALE(scene_bottom, mg1::WindowConstants::root_win_h);
std::vector<QuadVertex> quad{ { { quad_left, quad_bottom }, { 0, 1 } },
                              { { quad_right, quad_bottom }, { 1, 1 } },
                              { { quad_right, quad_top }, { 1, 0 } },
                              { { quad_left, quad_top }, { 0, 0 } } };
std::vector<uint32_t> quad_idx{ 0, 1, 2, 2, 3, 0 };

namespace mg1
{
  CadLayer::CadLayer() : m_scene{ Scene::create() }
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

    // create cameras
    {
      m_orbit_camera = std::make_shared<OrbitCameraController>();
      m_orbit_camera->set_sensitivity(1.f / 4.f);
      m_orbit_camera->set_perspective(EspWorkOrchestrator::get_swap_chain_extent_aspect_ratio());
      m_fps_camera = std::make_shared<FpsCameraController>();
      m_fps_camera->set_sensitivity(2.f);
      m_fps_camera->set_move_speed(4.f);
      m_fps_camera->set_perspective(EspWorkOrchestrator::get_swap_chain_extent_aspect_ratio());
      m_selected_camera = m_orbit_camera.get();
    }

    // create children layers
    {
      m_gui_layer    = std::unique_ptr<Layer>(new GuiLayer());
      m_object_layer = std::unique_ptr<Layer>(new ObjectLayer(m_scene.get()));
    }
  }

  void CadLayer::pre_update(float dt)
  {
    m_gui_layer->pre_update(dt);
    m_object_layer->pre_update(dt);
  }

  void CadLayer::update(float dt)
  {
    TransformManager::set_scene(m_scene.get());
    NodeManager::set_scene(m_scene.get());
    CadRenderer::set_camera(m_selected_camera);
    CadRenderer::set_scene(m_scene.get());

    m_orbit_camera->update(dt);
    m_fps_camera->update(dt);
    Math::update();

    if (EspGui::m_use_gui)
    {
      EspGui::new_frame();
      ImGui::SetNextWindowBgAlpha(0.f);
      EspGui::begin(ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
      ImGui::SetWindowPos(ImVec2(0, 0));
      ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);

      static bool show_demo_window = false;

      // Top menu bar
      if (ImGui::BeginMenuBar())
      {
        if (ImGui::BeginMenu("File")) { ImGui::EndMenu(); }
        if (ImGui::BeginMenu("View"))
        {
          ImGui::MenuItem("Show Demo Window", nullptr, &show_demo_window);
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }

      ImGui::BeginChild("Gui", WindowConstants::gui_win_size, ImGuiChildFlags_Border, ImGuiWindowFlags_NoTitleBar);
      m_gui_layer->update(dt);
      ImGui::EndChild();

      ImGui::SameLine(0.f, WindowConstants::gui_scene_splitter_size);

      ImGui::BeginChild("Scene", WindowConstants::scene_win_size, ImGuiChildFlags_Border, ImGuiWindowFlags_NoTitleBar);
      if (show_demo_window) { ImGui::ShowDemoWindow(); }
      ImGui::EndChild();

      EspGui::end();
      EspGui::end_frame();
    }

    m_object_layer->update(dt);

    if (m_anaglyph_mode.m_on)
    {
      m_selected_camera->set_anaglyph_perspective(m_anaglyph_mode.m_eye_dist, m_anaglyph_mode.m_plane_dist, true);
    }
    else { m_selected_camera->set_perspective(EspWorkOrchestrator::get_swap_chain_extent_aspect_ratio()); }

    // scene render plan [OFF-SCREEN]
    m_scene_render.m_plan->begin_plan();
    {
      CadRenderer::begin_scene(m_selected_camera);
      CadRenderer::render();
      CadRenderer::end_scene();
    }
    m_scene_render.m_plan->end_plan();
    m_scene_render.m_depth_block->clear();

    if (m_anaglyph_mode.m_on)
    {
      EspWorkOrchestrator::split_frame();

      m_selected_camera->set_anaglyph_perspective(m_anaglyph_mode.m_eye_dist, m_anaglyph_mode.m_plane_dist, false);

      // anaglyph scene render plan [OFF-SCREEN]
      m_anaglyph_mode_render.m_plan->begin_plan();
      {
        CadRenderer::begin_scene(m_selected_camera);
        CadRenderer::render();
        CadRenderer::end_scene();
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
    m_gui_layer->post_update(dt);
    m_object_layer->post_update(dt);
  }

  void CadLayer::handle_event(esp::Event& event, float dt)
  {
    m_gui_layer->handle_event(event, dt);
    m_object_layer->handle_event(event, dt);

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

    if (orbit_camera_selected()) { m_orbit_camera->handle_event(event, dt); }
    if (fps_camera_selected()) { m_fps_camera->handle_event(event, dt); }

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
      m_selected_camera = m_fps_camera.get();
      break;
    }
    case Orbit:
    {
      m_selected_camera = m_orbit_camera.get();
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
} // namespace mg1