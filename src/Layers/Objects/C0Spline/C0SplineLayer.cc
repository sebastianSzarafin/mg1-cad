#include "C0SplineLayer.hh"
#include "Layers/Objects/C0Spline/C0SplineComponent.hh"
#include "Layers/Objects/ControlLine/ControlLineComponent.hh"
#include "Layers/Objects/ObjectFactory.hh"

namespace mg1
{
  C0SplineLayer::C0SplineLayer(Scene* scene) : m_scene{ scene } {}

  void C0SplineLayer::pre_update(float dt)
  {
    for (auto&& [entity, obj, cl, model] : m_scene->get_view<C0SplineComponent, ControlLineComponent, ModelComponent>())
    {
      if (obj.get_info()->removed()) { ObjectFactory::remove_object(obj); }
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
            auto cl_indices = cl.get_indices();
            model.get_model(1).set_vertex_buffer(vertices);
            model.get_model(1).set_index_buffer(cl_indices, 0);
          }
        }

        obj.set_dirty_flag();
      }
    }
  }

  void C0SplineLayer::update(float dt)
  {
    for (auto&& [entity, obj, cl, model] : m_scene->get_view<C0SplineComponent, ControlLineComponent, ModelComponent>())
    {
      auto& obj_uniform_manager = model.get_uniform_manager(0);
      auto& cl_uniform_manager  = model.get_uniform_manager(1);

      if (obj.display_control_line())
      {
        model.choose({ 0, 1 });

        glm::vec3 color = ObjectConstants::bernstein_point_color;
        cl_uniform_manager.update_buffer_uniform(0, 1, 0, sizeof(glm::vec3), &color);
      }
      else { model.choose({ 0 }); }

      glm::vec3 color = obj.get_info()->selected() ? ObjectConstants::selected_color : ObjectConstants::default_color;
      obj_uniform_manager.update_buffer_uniform(0, 1, 0, sizeof(glm::vec3), &color);
    }
  }

  void C0SplineLayer::post_update(float dt)
  {
    static bool first_loop = true;
    if (first_loop)
    {
      // initial scene
      for (auto&& [entity, point] : m_scene->get_view<PointComponent>())
      {
        auto pos = point.get_position();
        if (pos.z <= -2) { point.get_info()->select(); }
      }
      ObjectFactory::create_c0_spline();
      for (auto&& [entity, point] : m_scene->get_view<PointComponent>())
      {
        point.get_info()->unselect();
      }
      first_loop = false;
    }
  }

  void C0SplineLayer::handle_event(Event& event, float dt)
  {
    Event::try_handler<GuiButtonClickedEvent>(event,
                                              ESP_BIND_EVENT_FOR_FUN(C0SplineLayer::gui_button_clicked_event_handler));
    Event::try_handler<MouseButtonPressedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(C0SplineLayer::mouse_button_pressed_event_handler));
    Event::try_handler<ObjectRemovedEvent>(event, ESP_BIND_EVENT_FOR_FUN(C0SplineLayer::object_removed_event_handler));
    Event::try_handler<GuiCheckboxChangedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(C0SplineLayer::gui_checkbox_changed_event_handler));
    Event::try_handler<CursorRotChangedEvent>(event,
                                              ESP_BIND_EVENT_FOR_FUN(C0SplineLayer::cursor_rot_changed_event_handler));
    Event::try_handler<CursorScaleChangedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(C0SplineLayer::cursor_scale_changed_event_handler));
  }

  bool C0SplineLayer::gui_button_clicked_event_handler(GuiButtonClickedEvent& event)
  {
    if (event == GuiLabel::create_c0_spline_button) { auto& spline = ObjectFactory::create_c0_spline(); }

    return false;
  }

  bool C0SplineLayer::mouse_button_pressed_event_handler(MouseButtonPressedEvent& event)
  {
    if (event.get_button_code() != GLFW_MOUSE_BUTTON_MIDDLE) { return false; }

    std::vector<PointComponent> clicked_points{};
    for (auto&& [entity, point] : m_scene->get_view<PointComponent>())
    {
      if (point.clicked()) { clicked_points.emplace_back(point); }
    }

    for (auto&& [entity, spline] : m_scene->get_view<C0SplineComponent>())
    {
      if (spline.get_info()->selected())
      {
        for (auto& point : clicked_points)
        {
          spline.push_back(point);
        }
      }
    }

    return false;
  }

  bool C0SplineLayer::object_removed_event_handler(ObjectRemovedEvent& event)
  {
    if (!(event == ObjectLabel::object_removed_event)) { return false; }

    for (auto&& [entity, obj] : m_scene->get_view<C0SplineComponent>())
    {
      obj.handle_event(event);
    }

    return false;
  }

  bool C0SplineLayer::gui_checkbox_changed_event_handler(GuiCheckboxChangedEvent& event)
  {
    if (!(event == GuiLabel::control_line_checkbox)) { return false; }

    for (auto&& [entity, obj] : m_scene->get_view<C0SplineComponent>())
    {
      obj.handle_event(event);
    }

    return false;
  }

  bool C0SplineLayer::cursor_rot_changed_event_handler(CursorRotChangedEvent& event)
  {
    if (!(event == ObjectLabel::cursor_rot_changed_event) || event.is_type(CursorType::Mouse)) { return false; }

    for (auto&& [entity, obj] : m_scene->get_view<C0SplineComponent>())
    {
      obj.handle_event(event);
    }

    return false;
  }

  bool C0SplineLayer::cursor_scale_changed_event_handler(CursorScaleChangedEvent& event)
  {
    if (!(event == ObjectLabel::cursor_scale_changed_event) || event.is_type(CursorType::Mouse)) { return false; }

    for (auto&& [entity, obj] : m_scene->get_view<C0SplineComponent>())
    {
      obj.handle_event(event);
    }

    return false;
  }
} // namespace mg1