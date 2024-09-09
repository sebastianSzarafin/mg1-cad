#include "PointLayer.hh"
#include "Layers/Objects/Cursor/CursorComponent.hh"
#include "Layers/Objects/ObjectFactory.hh"
#include "Layers/Objects/Point/PointComponent.hh"

namespace mg1
{
  PointLayer::PointLayer(Scene* scene) : m_scene{ scene } {}

  void PointLayer::pre_update(float dt)
  {
    for (auto&& [entity, obj, model] : m_scene->get_view<PointComponent, ModelComponent>())
    {
      if (obj.get_info()->removed()) { ObjectFactory::remove_object(obj); }
      else if (obj.get_info()->m_dirty)
      {
        auto [vertices, indices] = obj.reconstruct();
        model.get_model().set_vertex_buffer(vertices);
        model.get_model().set_index_buffer(indices, 0);
      }
    }
  }

  void PointLayer::update(float dt)
  {
    for (auto&& [entity, obj, model] : m_scene->get_view<PointComponent, ModelComponent>())
    {
      auto& uniform_manager = model.get_uniform_manager();

      glm::vec3 color = ObjectConstants::default_color;
      if (obj.bernstein_point()) { color = ObjectConstants::bernstein_point_color; }
      if (obj.get_info()->selected()) { color = ObjectConstants::selected_color; }
      uniform_manager.update_buffer_uniform(0, 1, 0, sizeof(glm::vec3), &color);
    }
  }

  void PointLayer::post_update(float dt)
  {
    static bool first_loop = true;
    if (first_loop) // initial scene
    {
      // c0 spline
      ObjectFactory::create_point({ 3, 1, -4 });
      ObjectFactory::create_point({ 2, 1, -2 });
      ObjectFactory::create_point({ 1, 1, -4 });
      ObjectFactory::create_point({ 0, 1, -2 });
      ObjectFactory::create_point({ -1, 1, -3 });
      ObjectFactory::create_point({ -2, 1, -3 });
      ObjectFactory::create_point({ -3, 1, -3 });
      // c2 spline
      ObjectFactory::create_point({ 3, 1, -1 });
      ObjectFactory::create_point({ 2, 1, 1 });
      ObjectFactory::create_point({ 1, 1, -1 });
      ObjectFactory::create_point({ 0, 1, 1 });
      ObjectFactory::create_point({ -1, 1, 0 });
      ObjectFactory::create_point({ -2, 1, 0 });
      ObjectFactory::create_point({ -3, 1, 0 });
      // c2 interpolation spline
      ObjectFactory::create_point({ 3, 1, 2 });
      ObjectFactory::create_point({ 2, 1, 4 });
      ObjectFactory::create_point({ 1, 1, 2 });
      ObjectFactory::create_point({ 0, 1, 4 });
      ObjectFactory::create_point({ -1, 1, 3 });
      ObjectFactory::create_point({ -2, 1, 3 });
      ObjectFactory::create_point({ -3, 1, 3 });

      first_loop = false;
    }
  }

  void PointLayer::handle_event(Event& event, float dt)
  {
    Event::try_handler<GuiToggleButtonClickedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(PointLayer::gui_toggle_button_clicked_event_handler));
    Event::try_handler<MouseButtonPressedEvent>(event,
                                                ESP_BIND_EVENT_FOR_FUN(PointLayer::mouse_button_pressed_event_handler));
    Event::try_handler<CursorPosChangedEvent>(event, ESP_BIND_EVENT_FOR_FUN(PointLayer::cursor_pos_changed_event));
  }

  bool PointLayer::gui_toggle_button_clicked_event_handler(GuiToggleButtonClickedEvent& event)
  {
    if (event == GuiLabel::create_point_button) { m_create_point_toggle_on = event.get_state(); }

    return false;
  }

  bool PointLayer::mouse_button_pressed_event_handler(MouseButtonPressedEvent& event)
  {
    if (event.get_button_code() == GLFW_MOUSE_BUTTON_MIDDLE)
    {
      if (m_create_point_toggle_on) { ObjectFactory::create_point(get_cursor_pos()); }
    }

    for (auto&& [entity, obj] : m_scene->get_view<PointComponent>())
    {
      obj.handle_event(event);
    }

    return false;
  }

  bool PointLayer::cursor_pos_changed_event(CursorPosChangedEvent& event)
  {
    if (event != ObjectLabel::cursor_pos_changed_event) { return false; }

    for (auto&& [entity, obj] : m_scene->get_view<PointComponent>())
    {
      obj.handle_event(event);
    }

    return false;
  }

  glm::vec3 PointLayer::get_cursor_pos()
  {
    for (auto&& [entity, cursor] : m_scene->get_view<CursorComponent>())
    {
      if (cursor.is_type(CursorType::Mouse)) { return cursor.get_position(); }
    }

    return { 0, 0, 0 };
  }
} // namespace mg1