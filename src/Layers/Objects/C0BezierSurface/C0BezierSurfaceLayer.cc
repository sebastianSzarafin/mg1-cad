#include "C0BezierSurfaceLayer.hh"
#include "Layers/Objects/C0BezierSurface/C0BezierSurfaceComponent.hh"
#include "Layers/Objects/ControlLine/ControlLineComponent.hh"
#include "Layers/Objects/Cursor/CursorComponent.hh"
#include "Layers/Objects/ObjectFactory.hh"

namespace mg1
{
  C0BezierSurfaceLayer::C0BezierSurfaceLayer(esp::Scene* scene) : m_scene{ scene } {}

  void C0BezierSurfaceLayer::pre_update(float dt)
  {
    for (auto&& [entity, obj, cl, model] :
         m_scene->get_view<C0BezierSurfaceComponent, ControlLineComponent, ModelComponent>())
    {
      if (obj.get_info()->removed()) { obj.remove(); }
      else
      {
        if (obj.get_info()->m_dirty)
        {
          auto [vertices, indices] = obj.reconstruct();
          model.get_model(0).set_vertex_buffer(vertices);
          model.get_model(0).set_index_buffer(indices, 0);

          if (obj.display_control_line())
          {
            cl.set_vertex_count(vertices.size());
            auto& cl_indices = obj.get_control_line_indices();
            model.get_model(1).set_vertex_buffer(vertices);
            model.get_model(1).set_index_buffer(cl_indices, 0);
          }
        }

        obj.set_dirty_flag();
      }
    }
  }

  void C0BezierSurfaceLayer::update(float dt)
  {
    for (auto&& [entity, obj, cl, model] :
         m_scene->get_view<C0BezierSurfaceComponent, ControlLineComponent, ModelComponent>())
    {
      auto& obj_uniform_manager = model.get_uniform_manager(0);
      auto& cl_uniform_manager  = model.get_uniform_manager(1);

      if (obj.display_control_line())
      {
        model.choose(1);

        glm::vec3 color = ObjectConstants::bernstein_point_color;
        cl_uniform_manager.update_buffer_uniform(0, 1, 0, sizeof(glm::vec3), &color);
      }
      else { model.skip(1); }

      auto color = obj.get_info()->selected() ? ObjectConstants::selected_color : ObjectConstants::default_color;
      obj_uniform_manager.update_buffer_uniform(0, 1, 0, sizeof(glm::vec3), &color);

      auto tess_level = obj.get_tesselation_level();
      auto tess_u     = std::get<0>(tess_level);
      auto tess_v     = std::get<1>(tess_level);
      obj_uniform_manager.update_buffer_uniform(1, 0, 0, sizeof(int), &tess_u);
      obj_uniform_manager.update_buffer_uniform(1, 0, sizeof(int), sizeof(int), &tess_v);
    }
  }

  void C0BezierSurfaceLayer::post_update(float dt) {}

  void C0BezierSurfaceLayer::handle_event(Event& event, float dt)
  {
    Event::try_handler<GuiSurfacePopupModalCreateButtonClickedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(C0BezierSurfaceLayer::gui_button_clicked_event_handler));
    Event::try_handler<GuiSelectableChangedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(C0BezierSurfaceLayer::gui_selectable_changed_event_handler));
    Event::try_handler<GuiCheckboxChangedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(C0BezierSurfaceLayer::gui_checkbox_changed_event_handler));
    Event::try_handler<CursorRotChangedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(C0BezierSurfaceLayer::cursor_rot_changed_event_handler));
    Event::try_handler<CursorScaleChangedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(C0BezierSurfaceLayer::cursor_scale_changed_event_handler));
  }

  bool C0BezierSurfaceLayer::gui_button_clicked_event_handler(GuiSurfacePopupModalCreateButtonClickedEvent& event)
  {
    if (event == GuiLabel::m_surface_popup_modal_create_button &&
        event.get_data().m_continuity == SurfaceContinuity::C0)
    {
      glm::vec3 position{ 0, 0, 0 };
      if (m_set_cursor_pos_action_selected) { position = get_cursor_pos(); }
      ObjectFactory::create_c0_bezier_surface(event.get_data(), position);
    }

    return false;
  }

  bool C0BezierSurfaceLayer::gui_selectable_changed_event_handler(GuiSelectableChangedEvent& event)
  {
    if (event == GuiLabel::action_set_cursor_pos) { m_set_cursor_pos_action_selected = event.get_value(); }

    return false;
  }

  bool C0BezierSurfaceLayer::gui_checkbox_changed_event_handler(GuiCheckboxChangedEvent& event)
  {
    if (!(event == GuiLabel::control_line_checkbox)) { return false; }

    for (auto&& [entity, obj] : m_scene->get_view<C0BezierSurfaceComponent>())
    {
      obj.handle_event(event);
    }

    return false;
  }

  bool C0BezierSurfaceLayer::cursor_rot_changed_event_handler(CursorRotChangedEvent& event)
  {
    if (!(event == ObjectLabel::cursor_rot_changed_event) || event.is_type(CursorType::Mouse)) { return false; }

    for (auto&& [entity, obj] : m_scene->get_view<C0BezierSurfaceComponent>())
    {
      obj.handle_event(event);
    }

    return false;
  }

  bool C0BezierSurfaceLayer::cursor_scale_changed_event_handler(CursorScaleChangedEvent& event)
  {
    if (!(event == ObjectLabel::cursor_scale_changed_event) || event.is_type(CursorType::Mouse)) { return false; }

    for (auto&& [entity, obj] : m_scene->get_view<C0BezierSurfaceComponent>())
    {
      obj.handle_event(event);
    }

    return false;
  }

  glm::vec3 C0BezierSurfaceLayer::get_cursor_pos()
  {
    for (auto&& [entity, cursor] : m_scene->get_view<CursorComponent>())
    {
      if (cursor.is_type(CursorType::Mouse)) { return cursor.get_position(); }
    }

    return { 0, 0, 0 };
  }
} // namespace mg1