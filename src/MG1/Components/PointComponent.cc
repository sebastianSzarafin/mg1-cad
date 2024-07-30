#include "PointComponent.hh"
#include "MG1/Events/Object/ObjectEvents.hh"
#include "MG1/Math.hh"

static void generate_point(float r, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

namespace mg1
{
  PointComponent::PointComponent(uint32_t id, bool bernstein_point, float r) : IComponent(id)
  {
    m_info      = std::make_shared<PointInfo>(m_id, "Point " + std::to_string(m_id));
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

    auto camera = Scene::get_current_camera();

    glm::vec3 ray_mouse =
        cast_ray(EspInput::get_mouse_x_cs(), EspInput::get_mouse_y_cs(), camera->get_view(), camera->get_projection());

    if (intersect_vector_sphere(camera->get_position(), ray_mouse, { { m_node->get_translation() }, m_info->m_r * 4 }))
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
      if (bernstein_point()) { m_node->translate(m_delta_position); }
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