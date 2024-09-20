#include "CursorLayer.hh"
#include "Events/Objects/ObjectEvents.hh"
#include "Layers/Objects/ObjectFactory.hh"
#include "Utils/Constants.hh"

namespace mg1
{
  CursorLayer::CursorLayer(Scene* scene) : m_scene{ scene } {}

  void CursorLayer::update(float dt)
  {
    if (!m_update) return;

    for (auto&& [entity, cursor, model] : m_scene->get_view<CursorComponent, ModelComponent>())
    {
      if (!cursor.get_info()->selected())
      {
        if (m_update_when_mouse_pressed)
        {
          if (!m_gui_captured) { cursor.update_when_mouse_pressed(); }
        }
        else { cursor.update(); }
      }

      if (cursor.visible()) { model.choose(0); }
      else { model.skip(0); }

      CursorPosChangedEvent event{ cursor.get_info()->m_type,
                                   cursor.get_info()->m_position,
                                   cursor.get_delta_position() };
      post_event(event);
    }
  }

  void CursorLayer::post_update(float dt)
  {
    static bool first_loop = true;
    if (first_loop)
    {
      // initial scene
      ObjectFactory::create_cursor(CursorType::Mouse).show();
      ObjectFactory::create_cursor(CursorType::Object).hide();
      first_loop = false;
    }
  }

  void CursorLayer::handle_event(esp::Event& event, float dt)
  {
    Event::try_handler<GuiMouseStateChangedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(CursorLayer::gui_mouse_state_changed_event_handler));
    Event::try_handler<ObjectMassCentreChangedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(CursorLayer::object_mass_centre_changed_event_handler));
    Event::try_handler<GuiSelectableChangedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(CursorLayer::gui_selectable_changed_event_handler));
    Event::try_handler<MouseMovedEvent>(event, ESP_BIND_EVENT_FOR_FUN(CursorLayer::mouse_moved_event_handler, dt));
    Event::try_handler<MouseScrolledEvent>(event, ESP_BIND_EVENT_FOR_FUN(CursorLayer::mouse_scrolled_event_handler));
    Event::try_handler<CursorPosChangedEvent>(event,
                                              ESP_BIND_EVENT_FOR_FUN(CursorLayer::cursor_pos_changed_event_handler));
  }

  bool CursorLayer::gui_mouse_state_changed_event_handler(mg1::GuiMouseStateChangedEvent& event)
  {
    m_gui_captured = (bool)event.get_state();

    return false;
  }

  bool CursorLayer::gui_selectable_changed_event_handler(GuiSelectableChangedEvent& event)
  {
    if (event == GuiLabel::action_rot_ox) { m_rotation_axis = event.get_value() ? RotationOX : RotationNone; }
    if (event == GuiLabel::action_rot_oy) { m_rotation_axis = event.get_value() ? RotationOY : RotationNone; }
    if (event == GuiLabel::action_rot_oz) { m_rotation_axis = event.get_value() ? RotationOZ : RotationNone; }
    if (event == GuiLabel::action_scale) { m_scale_axis = event.get_value() ? Scale : ScaleNone; }
    if (event == GuiLabel::action_scale_ox) { m_scale_axis = event.get_value() ? ScaleOX : ScaleNone; }
    if (event == GuiLabel::action_scale_oy) { m_scale_axis = event.get_value() ? ScaleOY : ScaleNone; }
    if (event == GuiLabel::action_scale_oz) { m_scale_axis = event.get_value() ? ScaleOZ : ScaleNone; }
    if (event == GuiLabel::action_set_cursor_pos) { m_update_when_mouse_pressed = event.get_value(); }

    return false;
  }

  bool CursorLayer::mouse_moved_event_handler(esp::MouseMovedEvent& event, float dt)
  {
    for (auto&& [entity, cursor] : m_scene->get_view<CursorComponent>())
    {
      cursor.handle_event(event, dt, m_rotation_axis);
    }

    return false;
  }

  bool CursorLayer::mouse_scrolled_event_handler(esp::MouseScrolledEvent& event)
  {
    for (auto&& [entity, cursor] : m_scene->get_view<CursorComponent>())
    {
      cursor.handle_event(event, m_scale_axis);
    }

    return false;
  }

  bool CursorLayer::object_mass_centre_changed_event_handler(mg1::ObjectMassCentreChangedEvent& event)
  {
    if (!(event == ObjectLabel::object_mass_centre_changed_event)) { return false; }

    for (auto&& [entity, cursor] : m_scene->get_view<CursorComponent>())
    {
      if (cursor.is_type(CursorType::Object))
      {
        if (event.create_or_update())
        {
          cursor.show();
          TransformManager::set_translation(entity, event.get_position(), action::ESP_RELATIVE);
          return false;
        }
        else
        {
          cursor.hide();
          TransformManager::set_scale(entity, { 1, 1, 1 });
          TransformManager::set_rotation(entity, { 1, 0, 0, 0 });
          return false;
        }
      }
    }

    return false;
  }

  bool CursorLayer::cursor_pos_changed_event_handler(CursorPosChangedEvent& event)
  {
    if (!(event == ObjectLabel::cursor_pos_changed_event && event.is_type(CursorType::Mouse))) { return false; }

    for (auto&& [entity, cursor] : m_scene->get_view<CursorComponent>())
    {
      if (cursor.is_type(CursorType::Object))
      {
        if (EspInput::is_mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT))
        {
          auto d_pos = event.get_delta_position();
          TransformManager::translate(entity, { d_pos.x, 0, 0 });
          if (EspInput::is_key_pressed(GLFW_KEY_Y)) { TransformManager::translate(entity, { 0, -d_pos.z, 0 }); }
          if (EspInput::is_key_pressed(GLFW_KEY_Z)) { TransformManager::translate(entity, { 0, 0, d_pos.z }); }
        }
        break;
      }
    }

    return false;
  }
} // namespace mg1