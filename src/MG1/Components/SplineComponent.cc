#include "SplineComponent.hh"

namespace mg1
{
  SplineComponent::SplineComponent(uint32_t id, Scene* scene, std::vector<PointComponent> control_points) :
      IComponent(id, scene)
  {
    m_info = std::make_shared<SplineInfo>(m_id, "Spline " + std::to_string(m_id), create_point_infos(control_points));

    ObjectAddedEvent e{ m_info.get() };
    post_event(e);
  }

  SplineComponent::SplineComponent(uint32_t id, esp::Scene* scene) : IComponent(id, scene) {}

  std::tuple<std::vector<Vertex>, std::vector<uint32_t>> SplineComponent::reconstruct()
  {
    std::vector<Vertex> vertices{};

    auto& points = m_info->m_control_points;

    auto size = points.size();
    vertices.resize(size);

    for (auto&& [entity, point] : m_scene->get_view<PointComponent>())
    {
      auto it  = std::find_if(points.begin(), points.end(), [&point](PointInfo* p) { return p == point.get_info(); });
      auto idx = std::distance(points.begin(), it);
      if (idx < 0 || idx >= size) { continue; }
      vertices[idx] = { point.get_node()->get_translation() };
    }

    m_info->m_dirty = false;

    return { vertices, get_spline_indices(size) };
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

  std::vector<PointInfo*> SplineComponent::create_point_infos(std::vector<PointComponent>& control_points)
  {
    std::vector<PointInfo*> infos{};

    std::transform(control_points.begin(),
                   control_points.end(),
                   std::back_inserter(infos),
                   [](PointComponent pc) { return pc.get_info(); });

    std::sort(infos.begin(),
              infos.end(),
              [](const PointInfo* p1, const PointInfo* p2) { return p1->m_selected_index < p2->m_selected_index; });

    return std::move(infos);
  }

  std::vector<uint32_t> SplineComponent::get_spline_indices(uint32_t vertex_count)
  {
    if (vertex_count <= 4) { return { 0, 1, 2, 3 }; }

    std::vector<uint32_t> indices{};
    for (auto i = 0; i < vertex_count; i++)
    {
      indices.push_back(i);
      if (i != 0 && i % 3 == 0) { indices.push_back(i); }
    }

    return indices;
  }
} // namespace mg1