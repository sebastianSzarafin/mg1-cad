#include "ObjectSelector.hh"
#include "Cursor/CursorComponent.hh"
#include "Point/PointComponent.hh"
#include "Torus/TorusComponent.hh"

namespace mg1
{
  ObjectSelector::ObjectSelector(Scene* scene) : IEventable(), m_scene{ scene } {}

  std::unique_ptr<ObjectSelector> ObjectSelector::create(Scene* scene)
  {
    return std::make_unique<ObjectSelector>(scene);
  }

  void ObjectSelector::pre_update()
  {
    for (auto&& [entity, obj, node] : m_scene->get_view<TorusComponent, NodeComponent>())
    {
      if (!obj.get_info()->selected()) { try_deselect_node(node); }
    }
    for (auto&& [entity, obj, node] : m_scene->get_view<PointComponent, NodeComponent>())
    {
      if (obj.bernstein_point()) { continue; }
      if (!obj.get_info()->selected()) { try_deselect_node(node); }
    }

    update_mass_centre();

    for (auto&& [entity, obj, node] : m_scene->get_view<TorusComponent, NodeComponent>())
    {
      if (obj.get_info()->selected()) { try_select_node(node); }
    }
    for (auto&& [entity, obj, node] : m_scene->get_view<PointComponent, NodeComponent>())
    {
      if (obj.bernstein_point()) { continue; }
      if (obj.get_info()->selected()) { try_select_node(node); }
    }
  }

  void ObjectSelector::try_select_node(NodeComponent& node)
  {
    // check if node is already selected, add if isn't
    if (!m_selected.try_add(node)) { return; }

    // update mass centre
    update_mass_centre();

    // reattach node
    for (auto&& [entity, cursor, cursor_node] : m_scene->get_view<CursorComponent, NodeComponent>())
    {
      if (cursor.is_type(CursorType::Object))
      {
        auto& parent = node.m_parent;
        // parent->add_child(&cursor_node);
        NodeComponent::get_node(parent).remove_child(node.m_handle);
        NodeComponent::get_node(cursor_node.m_handle).add_child(node.m_handle);

        return;
      }
    }
  }

  void ObjectSelector::try_deselect_node(NodeComponent& node)
  {
    // check if node is already selected, remove if is
    if (!m_selected.try_remove(node)) { return; }

    // reattach node
    for (auto&& [entity, cursor, cursor_node] : m_scene->get_view<CursorComponent, NodeComponent>())
    {
      if (cursor.is_type(CursorType::Object))
      {
        auto& parent       = cursor_node;
        auto& grand_parent = parent.m_parent;
        NodeComponent::get_node(parent.m_handle).remove_child(node.m_handle);
        NodeComponent::get_node(grand_parent).add_child(node.m_handle);

        return;
      }
    }
  }

  void ObjectSelector::update_mass_centre()
  {
    glm::vec3 mass_sum = { 0, 0, 0 };
    for (auto& selected : m_selected.m_nodes)
    {
      mass_sum += TransformManager::get_translation(selected);
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