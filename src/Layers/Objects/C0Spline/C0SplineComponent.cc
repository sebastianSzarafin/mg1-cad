#include "C0SplineComponent.hh"
#include "Layers/Objects/ObjectFactory.hh"

namespace mg1
{
  C0SplineComponent::C0SplineComponent(uint32_t id, Scene* scene, std::vector<PointComponent> control_points) :
      IComponent(id, scene)
  {
    sort_control_points(control_points);

    m_info =
        std::make_shared<C0SplineInfo>(m_id, "C0 Spline " + std::to_string(m_id), create_point_infos(control_points));

    m_control_points = create_control_points(control_points);

    ObjectAddedEvent e{ m_info.get() };
    post_event(e);
  }

  C0SplineComponent::C0SplineComponent(uint32_t id, esp::Scene* scene) : IComponent(id, scene) {}

  std::tuple<std::vector<Vertex>, std::vector<uint32_t>> C0SplineComponent::reconstruct()
  {
    auto size = m_control_points.size();

    std::vector<Vertex> vertices(size);

    auto idx = 0;
    for (auto& point : m_control_points)
    {
      vertices[idx++] = { ObjectFactory::get_control_point(point).get_position() };
    }

    m_info->m_dirty = false;

    return { vertices, get_spline_indices(size) };
  }

  std::vector<uint32_t> C0SplineComponent::get_spline_indices(uint32_t vertex_count)
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

  void C0SplineComponent::push_back(PointComponent& point)
  {
    m_info->m_control_points.push_back(point.get_info());
    m_control_points.push_back(point.get_id());
    m_info->m_dirty = true;
  }

  void C0SplineComponent::set_dirty_flag()
  {
    for (auto& point : m_control_points)
    {
      if (ObjectFactory::get_control_point(point).moved())
      {
        m_info->m_dirty = true;
        return;
      }
    }
  }

  void C0SplineComponent::handle_event(ObjectRemovedEvent& event)
  {
    // remove point from info's control points
    {
      auto it = std::find_if(m_info->m_control_points.begin(),
                             m_info->m_control_points.end(),
                             [&event](PointInfo* p) { return p == event.get_info(); });
      if (it != m_info->m_control_points.end())
      {
        m_info->m_control_points.erase(it);
        m_info->m_dirty = true;
      }
    }

    // remove point from spline's control points
    {
      auto it = std::find_if(m_control_points.begin(),
                             m_control_points.end(),
                             [&event](uint32_t& p) { return p == event.get_info()->m_id; });
      if (it != m_control_points.end()) { m_control_points.erase(it); }
    }
  }

  void C0SplineComponent::handle_event(GuiCheckboxChangedEvent& event)
  {
    if (m_info->selected()) { m_display_control_line = event.get_value(); }
  }

  void C0SplineComponent::handle_event(CursorRotChangedEvent& event)
  {
    if (m_info->selected()) { m_info->m_dirty = true; }
  }

  void C0SplineComponent::handle_event(CursorScaleChangedEvent& event)
  {
    if (m_info->selected()) { m_info->m_dirty = true; }
  }

  void C0SplineComponent::sort_control_points(std::vector<PointComponent>& control_points)
  {
    std::sort(control_points.begin(),
              control_points.end(),
              [](PointComponent& p1, PointComponent& p2)
              { return p1.get_info()->m_selected_index < p2.get_info()->m_selected_index; });
  }

  std::vector<uint32_t> C0SplineComponent::create_control_points(std::vector<PointComponent>& control_points)
  {
    std::vector<uint32_t> control_points_ids{};

    std::transform(control_points.begin(),
                   control_points.end(),
                   std::back_inserter(control_points_ids),
                   [](PointComponent& pc) { return pc.get_id(); });

    return std::move(control_points_ids);
  }

  std::vector<PointInfo*> C0SplineComponent::create_point_infos(std::vector<PointComponent>& control_points)
  {
    std::vector<PointInfo*> infos{};

    std::transform(control_points.begin(),
                   control_points.end(),
                   std::back_inserter(infos),
                   [](PointComponent& pc) { return pc.get_info(); });

    return std::move(infos);
  }
} // namespace mg1