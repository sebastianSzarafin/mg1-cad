#include "PointComponent.hh"
#include "Events/Objects/ObjectEvents.hh"
#include "Utils/Math.hh"

static void generate_point(float r, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

namespace mg1
{
  PointComponent::PointComponent(entt::entity id, bool bernstein_point, float r) : IComponent(id)
  {
    m_info      = std::make_shared<PointInfo>(get_id(), "Point " + std::to_string(get_id()));
    m_info->m_r = r;

    m_bernstein_point = bernstein_point;

    if (m_bernstein_point) { m_info->set_visibility(false); }

    ObjectAddedEvent e{ m_info.get() };
    post_event(e);
  }

  std::tuple<std::vector<Vertex>, std::vector<uint32_t>> PointComponent::reconstruct()
  {
    std::vector<Vertex> vertices{};
    std::vector<uint32_t> indices{};
    generate_point(m_info->m_r, vertices, indices);

    return { vertices, indices };
  }

  void PointComponent::handle_event(esp::MouseButtonPressedEvent& event)
  {
    auto button_code = event.get_button_code();
    if (button_code != GLFW_MOUSE_BUTTON_LEFT && button_code != GLFW_MOUSE_BUTTON_MIDDLE) { return; }

    auto camera     = CadRenderer::get_camera();
    auto camera_pos = camera->get_position();
    auto node_pos   = TransformManager::get_translation(m_id);
    auto eps        = .75f * std::log2(glm::length2(node_pos - camera_pos));
    if (Math::intersect_vector_sphere(camera_pos, Math::s_mouse_ray, { { node_pos }, m_info->m_r * eps }))
    {
      m_info->selected() ? m_info->unselect() : m_info->select();
      m_clicked = true;
    }
    else { m_clicked = false; }
  }

  void PointComponent::handle_event(CursorPosChangedEvent& event)
  {
    m_delta_position = { 0, 0, 0 };

    if (m_info->selected() && EspInput::is_mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT))
    {
      auto d_pos       = event.get_delta_position();
      m_delta_position = { d_pos.x,
                           EspInput::is_key_pressed(GLFW_KEY_Y) ? -d_pos.z : 0,
                           EspInput::is_key_pressed(GLFW_KEY_Z) ? d_pos.z : 0 };
    }
  }
} // namespace mg1

static void generate_point(float r, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
  const int stacks = 20; // Number of stacks (horizontal slices)
  const int slices = 20; // Number of slices (vertical segments)

  for (int i = 0; i <= stacks; ++i)
  {
    float phi = glm::pi<float>() * i / stacks;
    // float phiNext = glm::pi<float>() * (i + 1) / stacks;

    for (int j = 0; j <= slices; ++j)
    {
      float theta = 2 * glm::pi<float>() * j / slices;

      Vertex vertex;
      vertex.m_position.x = r * sin(phi) * cos(theta);
      vertex.m_position.y = r * cos(phi);
      vertex.m_position.z = r * sin(phi) * sin(theta);

      vertices.push_back(vertex);

      if (i < stacks && j < slices)
      {
        unsigned int idx      = i * (slices + 1) + j;
        unsigned int next_idx = idx + slices + 1;

        indices.push_back(idx);
        indices.push_back(next_idx);
        indices.push_back(idx + 1);

        indices.push_back(next_idx);
        indices.push_back(next_idx + 1);
        indices.push_back(idx + 1);
      }
    }
  }
}