#include "BezierCurveComponent.hh"

namespace mg1
{
  BezierCurveComponent::BezierCurveComponent(uint32_t id, std::vector<PointComponent> control_points) :
      IComponent(id), m_control_points{ control_points }
  {
    std::vector<ObjectInfo*> infos{};
    std::transform(control_points.begin(),
                   control_points.end(),
                   std::back_inserter(infos),
                   [](PointComponent pc) { return pc.get_info(); });

    m_info = std::make_shared<BezierCurveInfo>(m_id, "Bezier curve " + std::to_string(m_id), infos);

    ObjectAddedEvent e{ m_info.get() };
    post_event(e);
  }

  std::tuple<std::vector<Vertex>, std::vector<uint32_t>> BezierCurveComponent::reconstruct()
  {
    std::vector<Vertex> vertices{};
    std::vector<uint32_t> indices{};

    vertices.reserve(m_control_points.size());
    for (int i = 0; i < m_control_points.size(); i++)
    {
      vertices.emplace_back(m_control_points[i].get_node()->get_translation());
      indices.push_back(i);
    }

    return { vertices, indices };
  }
} // namespace mg1