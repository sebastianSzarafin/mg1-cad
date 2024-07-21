#include "C2SplineComponent.hh"
#include "MG1/Layers/Objects/ObjectFactory.hh"

namespace mg1
{
  C2SplineComponent::C2SplineComponent(uint32_t id, Scene* scene, std::vector<PointComponent> control_points) :
      SplineComponent(id, scene)
  {
    std::sort(control_points.begin(),
              control_points.end(),
              [](PointComponent& p1, PointComponent& p2)
              { return p1.get_info()->m_selected_index < p2.get_info()->m_selected_index; });

    m_info =
        std::make_shared<C2SplineInfo>(m_id, "C2 spline " + std::to_string(m_id), create_point_infos(control_points));

    m_control_points = control_points;

    auto vertices = create_bernstein_vertices();
    for (auto& vertex : vertices)
    {
      m_bernstein_control_points.emplace_back(ObjectFactory::create_bernstein_point(vertex.m_position));
    }

    ObjectAddedEvent e{ m_info.get() };
    post_event(e);
  }

  std::tuple<std::vector<Vertex>, std::vector<uint32_t>> C2SplineComponent::reconstruct()
  {
    auto vertices = create_bernstein_vertices();

    for (int i = 0; i < m_bernstein_control_points.size(); ++i)
    {
      m_bernstein_control_points[i].get_node()->set_translation(vertices[i].m_position);
    }

    m_info->m_dirty = false;

    return { vertices, get_spline_indices(vertices.size()) };
  }

  void C2SplineComponent::push_back(mg1::PointComponent& point)
  {
    SplineComponent::push_back(point);
    // TODO:
  }

  void C2SplineComponent::handle_event(ObjectRemovedEvent& event)
  {
    SplineComponent::handle_event(event);
    // TODO:
  }

  void C2SplineComponent::handle_event(GuiCheckboxChangedEvent& event) { SplineComponent::handle_event(event); }

  void C2SplineComponent::handle_event(GuiInputIntChangedEvent& event)
  {
    m_spline_base = (SplineBase)event.get_value();
  }

  std::vector<Vertex> C2SplineComponent::create_bernstein_vertices()
  {
    std::vector<Vertex> vertices{}; // vertices((vertices.size() - 3) * 3) (?)

    if (m_control_points.size() <= 4)
    {
      for (auto& point : m_control_points)
      {
        vertices.emplace_back(point.get_position());
      }

      return vertices;
    }

    glm::vec3 e = m_control_points[0].get_position();
    glm::vec3 f = m_control_points[1].get_position();
    glm::vec3 g = 1.f / 3.f * m_control_points[1].get_position() + 2.f / 3.f * m_control_points[2].get_position();

    vertices.emplace_back(e);
    vertices.emplace_back(f);
    vertices.emplace_back(g);

    for (auto i = 2; i < m_control_points.size() - 2; i++)
    {
      f = 2.f / 3.f * m_control_points[i].get_position() + 1.f / 3.f * m_control_points[i + 1].get_position();
      e = (f + g) / 2.f;
      g = 1.f / 3.f * m_control_points[i].get_position() + 2.f / 3.f * m_control_points[i + 1].get_position();

      vertices.emplace_back(e);
      vertices.emplace_back(f);
      if (i < m_control_points.size() - 3) { vertices.emplace_back(g); }
    }

    g = m_control_points[m_control_points.size() - 2].get_position();
    e = m_control_points[m_control_points.size() - 1].get_position();

    vertices.emplace_back(g);
    vertices.emplace_back(e);

    return std::move(vertices);
  }

} // namespace mg1