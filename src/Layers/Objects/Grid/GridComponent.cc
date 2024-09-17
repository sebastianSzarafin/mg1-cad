#include "GridComponent.hh"

static void generate_grid(std::vector<Vertex>& vertices,
                          std::vector<uint32_t>& indices,
                          mg1::GridComponentParams params);

namespace mg1
{

  GridComponent::GridComponent(uint32_t id) : IComponent(id) {}

  std::tuple<std::vector<Vertex>, std::vector<uint32_t>> GridComponent::construct(GridComponentParams params)
  {
    std::vector<Vertex> vertices{};
    std::vector<uint32_t> indices{};
    generate_grid(vertices, indices, params);

    return { vertices, indices };
  }
} // namespace mg1

static void generate_grid(std::vector<Vertex>& vertices,
                          std::vector<uint32_t>& indices,
                          mg1::GridComponentParams params)
{
  // OX
  int offset = vertices.size();
  for (int i = 0; i < params.m_threshold; i++)
  {
    Vertex v{};
    v.m_position = { i - params.m_threshold / 2, 0, 0 };
    v.m_color    = { 1, 0, 0 };

    int idx = i + offset;

    vertices.push_back(v);

    if (i < params.m_threshold - 2)
    {
      indices.push_back(idx);
      indices.push_back(idx + 1);
      indices.push_back(idx + 2);
    }
  }

  // OY
  offset = vertices.size();
  for (int i = 0; i < params.m_threshold; i++)
  {
    Vertex v{};
    v.m_position = { 0, 0, i - params.m_threshold / 2 };
    v.m_color    = { 0, 0, 1 };

    int idx = i + offset;

    vertices.push_back(v);

    if (i < params.m_threshold - 2)
    {
      indices.push_back(idx);
      indices.push_back(idx + 1);
      indices.push_back(idx + 2);
    }
  }

  offset = vertices.size();
  for (int i = 0; i < params.m_threshold; i++)
  {
    for (int j = 0; j < params.m_threshold; j++)
    {
      Vertex v{};
      v.m_position = { i - params.m_threshold / 2, params.m_y, j - params.m_threshold / 2 };
      v.m_color    = params.m_color;

      vertices.push_back(v);
    }
  }

  // columns
  for (int i = 0; i < params.m_threshold; i++)
  {
    for (int j = 0; j < params.m_threshold; j++)
    {
      int idx = i * params.m_threshold + j + offset;

      if (j < params.m_threshold - 2)
      {
        indices.push_back(idx);
        indices.push_back(idx + 1);
        indices.push_back(idx + 2);
      }
    }
  }

  // rows
  for (int i = 0; i < params.m_threshold; i++)
  {
    for (int j = 0; j < params.m_threshold; j++)
    {
      int idx = i * params.m_threshold + j + offset;

      if (i < params.m_threshold - 2)
      {
        indices.push_back(idx);
        indices.push_back(idx + params.m_threshold);
        indices.push_back(idx + params.m_threshold * 2);
      }
    }
  }

  for (auto& v : vertices)
  {
    v.m_position *= params.m_scale;
  }
}
