#include <utility>

#include "C2SplineComponent.hh"

static std::vector<Vertex> calculate_c2_vertices(std::vector<Vertex>& vertices);

namespace mg1
{

  C2SplineComponent::C2SplineComponent(uint32_t id, Scene* scene, std::vector<PointComponent> control_points) :
      SplineComponent(id, scene)
  {
    m_info =
        std::make_shared<C2SplineInfo>(m_id, "C2 spline " + std::to_string(m_id), create_point_infos(control_points));

    ObjectAddedEvent e{ m_info.get() };
    post_event(e);
  }

  std::tuple<std::vector<Vertex>, std::vector<uint32_t>> C2SplineComponent::reconstruct()
  {
    auto [vertices, indices] = SplineComponent::reconstruct();
    vertices                 = calculate_c2_vertices(vertices);

    return { vertices, get_spline_indices(vertices.size()) };
  }

  void C2SplineComponent::handle_event(ObjectAddedEvent& event) { SplineComponent::handle_event(event); }

  void C2SplineComponent::handle_event(ObjectRemovedEvent& event) { SplineComponent::handle_event(event); }

  void C2SplineComponent::handle_event(GuiCheckboxChangedEvent& event) { SplineComponent::handle_event(event); }

  void C2SplineComponent::handle_event(GuiInputIntChangedEvent& event)
  {
    m_spline_base = (SplineBase)event.get_value();
  }
} // namespace mg1

static std::vector<Vertex> calculate_c2_vertices(std::vector<Vertex>& vertices)
{
  if (vertices.size() <= 4) { return vertices; }

  std::vector<Vertex> vertices_c2{}; // vertices_c2((vertices.size() - 3) * 3) (?)

  glm::vec3 e = vertices[0].m_position;
  glm::vec3 f = vertices[1].m_position;
  glm::vec3 g = 1.f / 3.f * vertices[1].m_position + 2.f / 3.f * vertices[2].m_position;

  vertices_c2.emplace_back(e);
  vertices_c2.emplace_back(f);
  vertices_c2.emplace_back(g);

  for (auto i = 2; i < vertices.size() - 2; i++)
  {
    f = 2.f / 3.f * vertices[i].m_position + 1.f / 3.f * vertices[i + 1].m_position;
    e = (f + g) / 2.f;
    g = 1.f / 3.f * vertices[i].m_position + 2.f / 3.f * vertices[i + 1].m_position;

    vertices_c2.emplace_back(e);
    vertices_c2.emplace_back(f);
    if (i < vertices.size() - 3) { vertices_c2.emplace_back(g); }
  }

  g = vertices[vertices.size() - 2].m_position;
  //    f = 2.f / 3.f * vertices[vertices.size() - 3].m_position + 1.f / 3.f * vertices[vertices.size() -
  //    2].m_position;
  e = vertices[vertices.size() - 1].m_position;

  //    vertices_c2.emplace_back(f);
  vertices_c2.emplace_back(g);
  vertices_c2.emplace_back(e);

  return std::move(vertices_c2);
}