#include "C2InterpolationSplineLayer.hh"
#include "Layers/Objects/C2InterpolationSpline/C2InterpolationSplineComponent.hh"
#include "Layers/Objects/C2Spline/C2SplineComponent.hh"
#include "Layers/Objects/ObjectFactory.hh"

namespace mg1
{
  C2InterpolationSplineLayer::C2InterpolationSplineLayer(Scene* scene) : m_scene{ scene } {}

  void C2InterpolationSplineLayer::pre_update(float dt)
  {
    for (auto&& [entity, obj, model] : m_scene->get_view<C2InterpolationSplineComponent, ModelComponent>())
    {
      if (obj.get_info()->removed()) { ObjectFactory::remove_object(obj); }
      else
      {
        if (obj.get_info()->m_dirty)
        {
          auto [vertices, indices] = obj.reconstruct();
          model.get_model().set_vertex_buffer(vertices);
          model.get_model().set_index_buffer(indices, 0);
        }

        obj.set_dirty_flag();
      }
    }
  }

  void C2InterpolationSplineLayer::update(float dt)
  {
    for (auto&& [entity, obj, model] : m_scene->get_view<C2InterpolationSplineComponent, ModelComponent>())
    {
      auto& uniform_manager = model.get_uniform_manager();

      auto ubo = obj.get_ubo();
      uniform_manager.update_buffer_uniform(0, 1, 0, sizeof(C2SplineUbo), &ubo);

      SplineColorUbo color_ubo{};
      color_ubo.m_spline_color =
          obj.get_info()->selected() ? ObjectConstants::selected_color : ObjectConstants::default_color;
      uniform_manager.update_buffer_uniform(0, 2, 0, sizeof(SplineColorUbo), &color_ubo);
    }
  }

  void C2InterpolationSplineLayer::post_update(float dt)
  {
    static bool first_loop = true;
    if (first_loop)
    {
      // initial scene
      for (auto&& [entity, point] : m_scene->get_view<PointComponent>())
      {
        auto pos = point.get_position();
        if (pos.z >= 2) { point.get_info()->select(); }
      }
      ObjectFactory::create_c2_interpolation_spline();
      for (auto&& [entity, point] : m_scene->get_view<PointComponent>())
      {
        point.get_info()->unselect();
      }
      first_loop = false;
    }
  }

  void C2InterpolationSplineLayer::handle_event(Event& event, float dt)
  {
    Event::try_handler<GuiButtonClickedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(C2InterpolationSplineLayer::gui_button_clicked_event_handler));
    Event::try_handler<MouseButtonPressedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(C2InterpolationSplineLayer::mouse_button_pressed_event_handler));
    Event::try_handler<ObjectRemovedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(C2InterpolationSplineLayer::object_removed_event_handler));
    Event::try_handler<CursorRotChangedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(C2InterpolationSplineLayer::cursor_rot_changed_event_handler));
    Event::try_handler<CursorScaleChangedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(C2InterpolationSplineLayer::cursor_scale_changed_event_handler));
  }

  bool C2InterpolationSplineLayer::gui_button_clicked_event_handler(GuiButtonClickedEvent& event)
  {
    if (event == GuiLabel::create_c2_interpolation_spline_button) { ObjectFactory::create_c2_interpolation_spline(); }

    return false;
  }

  bool C2InterpolationSplineLayer::mouse_button_pressed_event_handler(MouseButtonPressedEvent& event)
  {
    if (event.get_button_code() != GLFW_MOUSE_BUTTON_MIDDLE) { return false; }

    std::vector<PointComponent> clicked_points{};
    for (auto&& [entity, point] : m_scene->get_view<PointComponent>())
    {
      if (point.clicked()) { clicked_points.emplace_back(point); }
    }

    for (auto&& [entity, spline] : m_scene->get_view<C2InterpolationSplineComponent>())
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

  bool C2InterpolationSplineLayer::object_removed_event_handler(ObjectRemovedEvent& event)
  {
    if (!(event == ObjectLabel::object_removed_event)) { return false; }

    for (auto&& [entity, obj] : m_scene->get_view<C2InterpolationSplineComponent>())
    {
      obj.handle_event(event);
    }

    return false;
  }

  bool C2InterpolationSplineLayer::cursor_rot_changed_event_handler(CursorRotChangedEvent& event)
  {
    if (!(event == ObjectLabel::cursor_rot_changed_event) || event.is_type(CursorType::Mouse)) { return false; }

    for (auto&& [entity, obj] : m_scene->get_view<C2InterpolationSplineComponent>())
    {
      obj.handle_event(event);
    }

    return false;
  }

  bool C2InterpolationSplineLayer::cursor_scale_changed_event_handler(CursorScaleChangedEvent& event)
  {
    if (!(event == ObjectLabel::cursor_scale_changed_event) || event.is_type(CursorType::Mouse)) { return false; }

    for (auto&& [entity, obj] : m_scene->get_view<C2InterpolationSplineComponent>())
    {
      obj.handle_event(event);
    }

    return false;
  }
} // namespace mg1