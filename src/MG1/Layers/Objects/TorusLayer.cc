#include "TorusLayer.hh"
#include "MG1/Components/Components.hh"
#include "ObjectFactory.hh"

namespace mg1
{
  TorusLayer::TorusLayer(Scene* scene) : m_scene{ scene } {}

  void TorusLayer::pre_update(float dt)
  {
    for (auto&& [entity, obj, model] : m_scene->get_view<TorusComponent, ModelComponent>())
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

  void TorusLayer::update(float dt)
  {
    auto camera = Scene::get_current_camera();

    for (auto&& [entity, obj, model] : m_scene->get_view<TorusComponent, ModelComponent>())
    {
      auto& uniform_manager = model.get_uniform_manager();
      glm::mat4 mvp         = camera->get_projection() * camera->get_view() * obj.get_node()->get_model_mat();
      uniform_manager.update_buffer_uniform(0, 0, 0, sizeof(glm::mat4), &mvp);

      glm::vec3 color = obj.get_info()->selected() ? ObjectConstants::selected_color : ObjectConstants::default_color;
      uniform_manager.update_buffer_uniform(0, 1, 0, sizeof(glm::vec3), &color);
    }
  }

  void TorusLayer::post_update(float dt)
  {
    static bool first_loop = true;
    if (first_loop)
    {
      // initial scene
      ObjectFactory::create_torus({ 2, 0, -5 });
      ObjectFactory::create_torus({ -2, 0, -5 });
      first_loop = false;
    }
  }

  void TorusLayer::handle_event(Event& event, float dt)
  {
    Event::try_handler<GuiToggleButtonClickedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(TorusLayer::gui_toggle_button_clicked_event_handler));
    Event::try_handler<MouseButtonPressedEvent>(event,
                                                ESP_BIND_EVENT_FOR_FUN(TorusLayer::mouse_button_pressed_event_handler));
  }

  bool TorusLayer::gui_toggle_button_clicked_event_handler(mg1::GuiToggleButtonClickedEvent& event)
  {
    if (event == GuiLabel::create_torus_button) { m_create_torus_toggle_on = event.get_state(); }

    return false;
  }

  bool TorusLayer::mouse_button_pressed_event_handler(MouseButtonPressedEvent& event)
  {
    if (event.get_button_code() == GLFW_MOUSE_BUTTON_MIDDLE)
    {
      if (m_create_torus_toggle_on) { ObjectFactory::create_torus(get_cursor_pos()); }
    }

    return false;
  }

  glm::vec3 TorusLayer::get_cursor_pos()
  {
    for (auto&& [entity, cursor] : m_scene->get_view<CursorComponent>())
    {
      if (cursor.is_type(CursorType::Mouse)) { return cursor.get_position(); }
    }

    return { 0, 0, 0 };
  }
} // namespace mg1