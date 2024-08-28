#include "SplineLayer.hh"
#include "Components/Components.hh"
#include "ObjectFactory.hh"

namespace mg1
{
  SplineLayer::SplineLayer(Scene* scene) : m_scene{ scene } {}

  void SplineLayer::pre_update(float dt)
  {
    for (auto&& [entity, obj, model] : m_scene->get_view<SplineComponent, ModelComponent>())
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

  void SplineLayer::update(float dt)
  {
    for (auto&& [entity, obj, model] : m_scene->get_view<SplineComponent, ModelComponent>())
    {
      auto& uniform_manager = model.get_uniform_manager();

      int control_line = obj.display_control_line();
      uniform_manager.update_buffer_uniform(0, 1, 0, sizeof(int), &control_line);

      SplineColorUbo ubo{};
      ubo.m_spline_color =
          obj.get_info()->selected() ? ObjectConstants::selected_color : ObjectConstants::default_color;
      uniform_manager.update_buffer_uniform(0, 2, 0, sizeof(SplineColorUbo), &ubo);
    }
  }

  void SplineLayer::post_update(float dt)
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
      ObjectFactory::create_spline();
      for (auto&& [entity, point] : m_scene->get_view<PointComponent>())
      {
        point.get_info()->unselect();
      }
      first_loop = false;
    }
  }

  void SplineLayer::handle_event(Event& event, float dt)
  {
    Event::try_handler<GuiButtonClickedEvent>(event,
                                              ESP_BIND_EVENT_FOR_FUN(SplineLayer::gui_button_clicked_event_handler));
    Event::try_handler<MouseButtonPressedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(SplineLayer::mouse_button_pressed_event_handler));
    Event::try_handler<ObjectRemovedEvent>(event, ESP_BIND_EVENT_FOR_FUN(SplineLayer::object_removed_event_handler));
    Event::try_handler<GuiCheckboxChangedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(SplineLayer::gui_checkbox_changed_event_handler));
  }

  bool SplineLayer::gui_button_clicked_event_handler(GuiButtonClickedEvent& event)
  {
    if (event == GuiLabel::create_spline_button) { ObjectFactory::create_spline(); }

    return false;
  }

  bool SplineLayer::mouse_button_pressed_event_handler(MouseButtonPressedEvent& event)
  {
    if (event.get_button_code() != GLFW_MOUSE_BUTTON_MIDDLE) { return false; }

    std::vector<PointComponent> clicked_points{};
    for (auto&& [entity, point] : m_scene->get_view<PointComponent>())
    {
      if (point.clicked()) { clicked_points.emplace_back(point); }
    }

    for (auto&& [entity, spline] : m_scene->get_view<SplineComponent>())
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

  bool SplineLayer::object_removed_event_handler(ObjectRemovedEvent& event)
  {
    if (!(event == ObjectLabel::object_removed_event)) { return false; }

    for (auto&& [entity, obj] : m_scene->get_view<SplineComponent>())
    {
      obj.handle_event(event);
    }

    return false;
  }

  bool SplineLayer::gui_checkbox_changed_event_handler(GuiCheckboxChangedEvent& event)
  {
    if (!(event == GuiLabel::control_line_checkbox)) { return false; }

    for (auto&& [entity, obj] : m_scene->get_view<SplineComponent>())
    {
      obj.handle_event(event);
    }

    return false;
  }
} // namespace mg1