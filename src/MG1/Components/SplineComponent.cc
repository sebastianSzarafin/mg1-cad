#include "SplineComponent.hh"

namespace mg1
{
  SplineComponent::SplineComponent(uint32_t id, Scene* scene, std::vector<PointComponent> control_points) :
      IComponent(id, scene)
  {
    std::vector<PointInfo*> infos{};
    std::transform(control_points.begin(),
                   control_points.end(),
                   std::back_inserter(infos),
                   [](PointComponent pc) { return pc.get_info(); });
    std::sort(infos.begin(),
              infos.end(),
              [](const PointInfo* p1, const PointInfo* p2) { return p1->m_selected_index < p2->m_selected_index; });

    m_info = std::make_shared<SplineInfo>(m_id, "Spline " + std::to_string(m_id), infos);

    ObjectAddedEvent e{ m_info.get() };
    post_event(e);
  }

  std::tuple<std::vector<Vertex>, std::vector<uint32_t>> SplineComponent::reconstruct()
  {
    std::vector<Vertex> vertices{};
    std::vector<uint32_t> indices{};

    auto& points = m_info->m_control_points;

    auto size = points.size();
    if (size > 4 && (size - 4) % 3 != 0) { size -= (size - 4) % 3; }

    vertices.resize(size);

    int i = 0;
    for (auto&& [entity, point] : m_scene->get_view<PointComponent>())
    {
      auto it  = std::find_if(points.begin(), points.end(), [&point](PointInfo* p) { return p == point.get_info(); });
      auto idx = std::distance(points.begin(), it);
      if (idx < 0 || idx >= size) { continue; }
      vertices[idx] = { point.get_node()->get_translation() };
      indices.push_back(i);
      if (i != 0 && i % 3 == 0) { indices.push_back(i); }
      i++;
    }

    m_info->m_dirty = false;

    return { vertices, indices };
  }

  void SplineComponent::push_back(PointComponent& point) { m_info->m_control_points.push_back(point.get_info()); }

  void SplineComponent::handle_event(ObjectAddedEvent& event)
  {
    auto* info = dynamic_cast<PointInfo*>(event.get_info());

    if (info) { m_info->m_control_points.push_back(info); }
  }

  void SplineComponent::handle_event(ObjectRemovedEvent& event)
  {
    auto& points = m_info->m_control_points;

    auto it = std::find_if(points.begin(), points.end(), [&event](PointInfo* p) { return p == event.get_info(); });
    if (it != points.end())
    {
      points.erase(it);
      m_info->m_dirty = true;
    }
  }

  void SplineComponent::handle_event(GuiCheckboxChangedEvent& event) { m_display_control_line = event.get_value(); }
} // namespace mg1