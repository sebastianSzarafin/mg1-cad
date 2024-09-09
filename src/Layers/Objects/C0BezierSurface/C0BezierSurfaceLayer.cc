#include "C0BezierSurfaceLayer.hh"
#include "Layers/Objects/Components.hh"
#include "Layers/Objects/ObjectFactory.hh"

namespace mg1
{
  C0BezierSurfaceLayer::C0BezierSurfaceLayer(esp::Scene* scene) : m_scene{ scene } {}

  void C0BezierSurfaceLayer::pre_update(float dt)
  {
    for (auto&& [entity, obj, model] : m_scene->get_view<C0BezierSurfaceComponent, ModelComponent>())
    {
      if (obj.get_info()->removed()) { obj.remove(); }
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

  void C0BezierSurfaceLayer::update(float dt)
  {
    for (auto&& [entity, obj, model] : m_scene->get_view<C0BezierSurfaceComponent, ModelComponent>())
    {
      auto& uniform_manager = model.get_uniform_manager();

      auto color = obj.get_info()->selected() ? ObjectConstants::selected_color : ObjectConstants::default_color;
      uniform_manager.update_buffer_uniform(0, 1, 0, sizeof(glm::vec3), &color);

      auto tess_level = obj.get_tesselation_level();
      auto tess_u     = std::get<0>(tess_level);
      auto tess_v     = std::get<1>(tess_level);
      uniform_manager.update_buffer_uniform(1, 0, 0, sizeof(int), &tess_u);
      uniform_manager.update_buffer_uniform(1, 0, sizeof(int), sizeof(int), &tess_v);
    }
  }

  void C0BezierSurfaceLayer::post_update(float dt) {}

  void C0BezierSurfaceLayer::handle_event(Event& event, float dt)
  {
    Event::try_handler<GuiSurfacePopupModalCreateButtonClickedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(C0BezierSurfaceLayer::gui_button_clicked_event_handler));
    Event::try_handler<GuiCheckboxChangedEvent>(
        event,
        ESP_BIND_EVENT_FOR_FUN(C0BezierSurfaceLayer::gui_checkbox_changed_event_handler));
  }

  bool C0BezierSurfaceLayer::gui_button_clicked_event_handler(GuiSurfacePopupModalCreateButtonClickedEvent& event)
  {
    if (event == GuiLabel::m_surface_popup_modal_create_button)
    {
      ObjectFactory::create_c0_bezier_surface(event.get_data());
    }

    return false;
  }

  bool C0BezierSurfaceLayer::gui_checkbox_changed_event_handler(GuiCheckboxChangedEvent& event) { return false; }
} // namespace mg1