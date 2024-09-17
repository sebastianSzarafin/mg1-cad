#include "GridLayer.hh"
#include "Layers/Objects/ObjectFactory.hh"
#include "Utils/Constants.hh"

namespace mg1
{
  GridLayer::GridLayer(Scene* scene) : m_scene{ scene } {}

  void GridLayer::pre_update(float dt) {}

  void GridLayer::update(float dt) {}

  void GridLayer::post_update(float dt)
  {
    static bool first_loop = true;
    if (first_loop)
    {
      // initial scene
      ObjectFactory::create_grid({ 30, 3, { 1, 1, 1 }, 0 });
      ObjectFactory::create_grid({ 400, .25f, { .25f, .25f, .25f }, -.001f });
      first_loop = false;
    }
  }

  void GridLayer::handle_event(esp::Event& event, float dt)
  {
    Event::try_handler<GuiCheckboxChangedEvent>(event,
                                                ESP_BIND_EVENT_FOR_FUN(GridLayer::gui_checkbox_changed_event_handler));
  }

  bool GridLayer::gui_checkbox_changed_event_handler(mg1::GuiCheckboxChangedEvent& event)
  {
    if (!(event == GuiLabel::grid_checkbox)) { return false; }
    if (event.get_value()) { push_grid(); }
    else { pop_grid(); }

    return true;
  }

  void GridLayer::push_grid()
  {
    for (auto&& [entity, grid] : m_scene->get_view<GridComponent>())
    {
      m_scene->get_root().add_child(grid.get_node());
    }
  }

  void GridLayer::pop_grid()
  {
    for (auto&& [entity, grid] : m_scene->get_view<GridComponent>())
    {
      m_scene->get_root().remove_child(grid.get_node());
    }
  }
} // namespace mg1