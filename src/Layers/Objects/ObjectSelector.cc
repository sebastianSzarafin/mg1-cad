#include "ObjectSelector.hh"

namespace mg1
{
  ObjectSelector::ObjectSelector(Scene* scene) : IEventable(), m_scene{ scene } {}

  std::unique_ptr<ObjectSelector> ObjectSelector::create(Scene* scene)
  {
    return std::make_unique<ObjectSelector>(scene);
  }

  void ObjectSelector::pre_update()
  {
    for (auto&& [entity, obj] : m_scene->get_view<TorusComponent>())
    {
      if (!obj.get_info()->selected()) { try_deselect_node(obj.get_node()); }
    }
    for (auto&& [entity, obj] : m_scene->get_view<PointComponent>())
    {
      if (obj.bernstein_point()) { continue; }
      if (!obj.get_info()->selected()) { try_deselect_node(obj.get_node()); }
    }

    update_mass_centre();

    for (auto&& [entity, obj] : m_scene->get_view<TorusComponent>())
    {
      if (obj.get_info()->selected()) { try_select_node(obj.get_node()); }
    }
    for (auto&& [entity, obj] : m_scene->get_view<PointComponent>())
    {
      if (obj.bernstein_point()) { continue; }
      if (obj.get_info()->selected()) { try_select_node(obj.get_node()); }
    }
  }

  void ObjectSelector::try_select_node(Node* node)
  {
    // check if node is already selected, add if isn't
    if (!m_selected.try_add(node)) { return; }

    // update mass centre
    update_mass_centre();

    // reattach node
    Node* cursor_node;
    for (auto&& [entity, cursor] : m_scene->get_view<CursorComponent>())
    {
      if (cursor.is_type(CursorType::Object))
      {
        cursor_node = cursor.get_node();
        break;
      }
    }

    if (cursor_node)
    {
      auto parent = node->get_parent();
      parent->add_child(cursor_node);
      parent->remove_child(node);
      cursor_node->add_child(node);
    }
  }

  void ObjectSelector::try_deselect_node(Node* node)
  {
    // check if node is already selected, remove if is
    if (!m_selected.try_remove(node)) { return; }

    // reattach node
    Node* cursor_node = nullptr;
    for (auto&& [entity, cursor] : m_scene->get_view<CursorComponent>())
    {
      if (cursor.is_type(CursorType::Object))
      {
        cursor_node = cursor.get_node();
        break;
      }
    }

    if (cursor_node)
    {
      auto parent       = cursor_node;
      auto grand_parent = parent->get_parent();
      parent->remove_child(node);
      grand_parent->add_child(node);
    }
  }

  void ObjectSelector::update_mass_centre()
  {
    glm::vec3 mass_sum = { 0, 0, 0 };
    for (auto& selected : m_selected.m_nodes)
    {
      mass_sum += selected->get_translation();
    }
    glm::vec3 new_mass_centre = mass_sum / (float)m_selected.m_nodes.size();
    if (m_selected.m_mass_centre != new_mass_centre)
    {
      m_selected.m_mass_centre = new_mass_centre;
      ObjectMassCentreChangedEvent event{ !m_selected.m_nodes.empty(), m_selected.m_mass_centre };
      post_event(event);
    }
  }
} // namespace mg1