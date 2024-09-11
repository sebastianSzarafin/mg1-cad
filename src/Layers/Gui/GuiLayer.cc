#include "GuiLayer.hh"
#include "Utils/Math.hh"

namespace mg1
{
  GuiLayer::GuiLayer()
  {
    m_grid_checkbox               = std::make_unique<GuiCheckbox>(GuiLabel::grid_checkbox, true);
    m_anaglyph_mode_checkbox      = std::make_unique<GuiCheckbox>(GuiLabel::anaglyph_mode, false);
    m_eye_distance_float_slider   = std::make_unique<GuiFloatSlider>(GuiLabel::m_eye_distance_float_slider, .01f, 0, 2);
    m_plane_distance_float_slider = std::make_unique<GuiFloatSlider>(GuiLabel::m_plane_distance_float_slider, 1, 0, 2);
    m_actions_combo               = std::make_unique<GuiSelectableCombo<bool>>(
        GuiLabel::actions,
        GuiSelectables<bool>{ std::make_shared<GuiSelectable<bool>>(GuiLabel::action_none, true, true),
                                            std::make_shared<GuiSelectable<bool>>(GuiLabel::action_rot_ox, false, false),
                                            std::make_shared<GuiSelectable<bool>>(GuiLabel::action_rot_oy, false, false),
                                            std::make_shared<GuiSelectable<bool>>(GuiLabel::action_rot_oz, false, false),
                                            std::make_shared<GuiSelectable<bool>>(GuiLabel::action_scale, false, false),
                                            std::make_shared<GuiSelectable<bool>>(GuiLabel::action_scale_ox, false, false),
                                            std::make_shared<GuiSelectable<bool>>(GuiLabel::action_scale_oy, false, false),
                                            std::make_shared<GuiSelectable<bool>>(GuiLabel::action_scale_oz, false, false),
                                            std::make_shared<GuiSelectable<bool>>(GuiLabel::action_set_cursor_pos, false, false) },
        ImGuiComboFlags_HeightLargest);
    m_hide_points_checkbox = std::make_unique<GuiCheckbox>(GuiLabel::hide_points_checkbox, false);
    m_objects_list_box     = std::make_unique<GuiObjectInfoSelectableListBox>(
        GuiLabel::objects,
        GuiSelectables<ObjectInfo*>{
            std::make_shared<GuiSelectable<ObjectInfo*>>(GuiLabel::object_none, nullptr, true),
        });
    m_create_torus_button = std::make_unique<GuiToggleButton>(GuiLabel::create_torus_button);
    m_create_torus_button->set_max_width();
    m_create_point_button = std::make_unique<GuiToggleButton>(GuiLabel::create_point_button);
    m_create_point_button->set_max_width();
    m_create_c0_spline_button = std::make_unique<GuiButton>(GuiLabel::create_c0_spline_button);
    m_create_c0_spline_button->set_max_width();
    m_create_c2_spline_button = std::make_unique<GuiButton>(GuiLabel::create_c2_spline_button);
    m_create_c2_spline_button->set_max_width();
    m_create_c2_interpolation_spline_button =
        std::make_unique<GuiButton>(GuiLabel::create_c2_interpolation_spline_button);
    m_create_c2_interpolation_spline_button->set_max_width();
    m_create_surface_popup_modal = std::make_unique<GuiSurfacePopupModal>(ImGuiWindowFlags_AlwaysAutoResize);
  }

  void GuiLayer::update(float dt)
  {
    if (!EspGui::m_use_gui) return;

    update_mouse_state();

    static int frames_count = 0;
    static float fps_count  = 0;

    frames_count++;
    fps_count += 1.f / dt;
    ImGui::Text("FPS: %.1f (avg: %.1f)", 1.f / dt, fps_count / frames_count);
    ImGui::Spacing();
    int type = (int)m_camera_type;
    ImGui::RadioButton("Orbit Camera", &type, (int)CameraType::Orbit);
    ImGui::RadioButton("Fps Camera", &type, (int)CameraType::Fps);
    ImGui::Spacing();

    update_camera_type((CameraType)type);

    glm::vec3 pos = Scene::get_current_camera()->get_position();
    ImGui::Text("Camera pos: (%.2f,%.2f,%.2f)", pos.x, pos.y, pos.z);
    ImGui::Spacing();
    ImGui::Text("Mouse pos: (%.2f,%.2f)", Math::get_mouse_x_cs(), Math::get_mouse_y_cs());
    ImGui::Spacing();
    ImGui::Text("Cursor pos: (%.2f,%.2f,%.2f)", m_mouse_cursor_pos.x, m_mouse_cursor_pos.y, m_mouse_cursor_pos.z);
    ImGui::Spacing();
    m_grid_checkbox->render();

    ImGui::SeparatorText("Stereoscopic visualisation:");
    m_anaglyph_mode_checkbox->render();
    if (m_anaglyph_mode_checkbox->get_value())
    {
      ImGui::PushItemWidth(WindowConstants::gui_win_size.x / 2);
      m_eye_distance_float_slider->render();
      m_plane_distance_float_slider->render();
      ImGui::PopItemWidth();
    }

    ImGui::SeparatorText("Actions:");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    m_actions_combo->render();

    ImGui::SeparatorText("Objects:");
    ImGui::Spacing();
    m_hide_points_checkbox->render();
    ImGui::Spacing();
    m_objects_list_box->render();
    m_create_torus_button->render();
    m_create_point_button->render();
    m_create_c0_spline_button->render();
    m_create_c2_spline_button->render();
    m_create_c2_interpolation_spline_button->render();
    if (ImGui::Button(GuiLabel::m_create_c0_surface_button.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0)))
    {
      m_create_surface_popup_modal->set_continuity(SurfaceContinuity::C0);
      m_create_surface_popup_modal->open();
    }
    m_create_surface_popup_modal->render();
  }

  void GuiLayer::handle_event(esp::Event& event, float dt)
  {
    Event::try_handler<ObjectAddedEvent>(event, ESP_BIND_EVENT_FOR_FUN(GuiLayer::object_added_event_handler));
    Event::try_handler<ObjectRemovedEvent>(event, ESP_BIND_EVENT_FOR_FUN(GuiLayer::object_removed_event_handler));
    Event::try_handler<CursorPosChangedEvent>(event,
                                              ESP_BIND_EVENT_FOR_FUN(GuiLayer::cursor_pos_changed_event_handler));
    Event::try_handler<GuiCheckboxChangedEvent>(event,
                                                ESP_BIND_EVENT_FOR_FUN(GuiLayer::gui_checkbox_changed_event_handler));
  }

  bool GuiLayer::object_added_event_handler(ObjectAddedEvent& event)
  {
    if (!(event == ObjectLabel::object_created_event)) { return false; }
    m_objects_list_box->handle_event(event);
    return true;
  }

  bool GuiLayer::object_removed_event_handler(mg1::ObjectRemovedEvent& event)
  {
    if (!(event == ObjectLabel::object_removed_event)) { return false; }
    m_objects_list_box->handle_event(event);
    return true;
  }

  bool GuiLayer::cursor_pos_changed_event_handler(CursorPosChangedEvent& event)
  {
    if (!(event == ObjectLabel::cursor_pos_changed_event && event.is_type(CursorType::Mouse))) { return false; }
    m_mouse_cursor_pos = event.get_position();

    return true;
  }

  bool GuiLayer::gui_checkbox_changed_event_handler(GuiCheckboxChangedEvent& event)
  {
    if (!(event == GuiLabel::hide_points_checkbox)) { return false; }

    m_objects_list_box->handle_event(event);

    return true;
  }

  void GuiLayer::update_mouse_state()
  {
    if (m_mouse_state == MouseState::GuiCaptured && EspInput::is_mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT))
    {
      return;
    }

    bool window_hovered =
        ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) || ImGui::IsWindowHovered(ImGuiHoveredFlags_RootWindow);
    // if (!window_hovered) { ImGui::SetMouseCursor(ImGuiMouseCursor_None); }
    if (m_mouse_state != window_hovered)
    {
      m_mouse_state = (MouseState)window_hovered;
      GuiMouseStateChangedEvent mouse_state_changed_event{ m_mouse_state };
      post_event(mouse_state_changed_event);
    }
  }

  void GuiLayer::update_camera_type(mg1::CameraType type)
  {
    if (m_camera_type != type)
    {
      m_camera_type = type;
      GuiCameraTypeChangedEvent event{ m_camera_type };
      post_event(event);
    }
  }

  void GuiLayer::help_marker(const char* desc)
  {
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
      ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
      ImGui::TextUnformatted(desc);
      ImGui::PopTextWrapPos();
      ImGui::EndTooltip();
    }
  }
} // namespace mg1