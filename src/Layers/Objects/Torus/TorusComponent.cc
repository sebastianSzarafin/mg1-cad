#include "TorusComponent.hh"
#include "Events/Objects/ObjectEvents.hh"

static std::vector<Vertex> generate_torus_vertices(float R, float r, int num_segments_theta, int num_segments_phi);
static std::vector<uint32_t> generate_torus_indices(int num_segments_theta, int num_segments_phi);

namespace mg1
{
  TorusComponent::TorusComponent(entt::entity id, float R, float r, int density_theta, int density_phi) : IComponent(id)
  {
    m_info                  = std::make_shared<TorusInfo>(get_id(), "Torus " + std::to_string(get_id()));
    m_info->m_R             = R;
    m_info->m_r             = r;
    m_info->m_density_theta = density_theta;
    m_info->m_density_phi   = density_phi;

    ObjectAddedEvent e{ m_info.get() };
    post_event(e);
  }

  std::tuple<std::vector<Vertex>, std::vector<uint32_t>> TorusComponent::reconstruct()
  {
    auto vertices = generate_torus_vertices(m_info->m_R, m_info->m_r, m_info->m_density_theta, m_info->m_density_phi);
    auto indices  = generate_torus_indices(m_info->m_density_theta, m_info->m_density_phi);

    m_info->m_dirty = false;

    return { vertices, indices };
  }

  void TorusComponent::handle_event(mg1::CursorPosChangedEvent& event)
  {
    if (EspInput::is_mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT))
    {
      auto d_pos = event.get_delta_position();
      TransformManager::translate(m_id, { d_pos.x, 0, 0 });
      if (EspInput::is_key_pressed(GLFW_KEY_Y)) { TransformManager::translate(m_id, { 0, -d_pos.z, 0 }); }
      if (EspInput::is_key_pressed(GLFW_KEY_Z)) { TransformManager::translate(m_id, { 0, 0, d_pos.z }); }
    }
  }
} // namespace mg1

static std::vector<Vertex> generate_torus_vertices(float R, float r, int num_segments_theta, int num_segments_phi)
{
  std::vector<Vertex> vertices{};

  float d_theta = 2.0f * M_PI / num_segments_theta;
  float d_phi   = 2.0f * M_PI / num_segments_phi;

  for (int i = 0; i < num_segments_theta; i++)
  {
    float theta = i * d_theta;

    for (int j = 0; j <= num_segments_phi; j++)
    {
      float phi = j * d_phi;

      float x = (R + r * std::cos(phi)) * std::cos(theta);
      float y = (R + r * std::cos(phi)) * std::sin(theta);
      float z = r * std::sin(phi);

      vertices.push_back({ { x, y, z } });
    }
  }

  return vertices;
}

static std::vector<uint32_t> generate_torus_indices(int num_segments_theta, int num_segments_phi)
{
  std::vector<uint32_t> indices{};

  for (int i = 0; i < num_segments_theta; ++i)
  {
    for (int j = 0; j < num_segments_phi; ++j)
    {
      int first  = i * (num_segments_phi + 1) + j;
      int second = (first + num_segments_phi + 1) % ((num_segments_theta) * (num_segments_phi + 1));
      int third  = (first + 1) % ((num_segments_theta) * (num_segments_phi + 1));
      int fourth = (second + 1) % ((num_segments_theta) * (num_segments_phi + 1));

      indices.push_back(first);
      indices.push_back(second);
      indices.push_back(third);

      indices.push_back(second);
      indices.push_back(fourth);
      indices.push_back(third);
    }
  }

  return indices;
}