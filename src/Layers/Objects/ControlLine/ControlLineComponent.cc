#include "ControlLineComponent.hh"

namespace mg1
{
  ControlLineComponent::ControlLineComponent(entt::entity id, uint32_t vertex_count) :
      IComponent(id), m_vertex_count{ vertex_count }
  {
  }

  std::vector<uint32_t> ControlLineComponent::get_indices()
  {
    std::vector<uint32_t> indices{};
    for (auto i = 0; i < m_vertex_count - 1; i++)
    {
      indices.push_back(i);
      indices.push_back(i + 1);
    }

    return indices;
  }
} // namespace mg1