#include "CursorComponent.hh"
#include "Events/Objects/ObjectEvents.hh"
#include "Utils/Math.hh"

static mg1::Math::Plane scene_plane = { { 0, 1, 0 }, 0 };
static float cursor_size            = .5f;
static glm::vec3 cursor_color_ox    = { 1, 0, 0 };
static glm::vec3 cursor_color_oy    = { 0, 1, 0 };
static glm::vec3 cursor_color_oz    = { 0, 0, 1 };

static std::vector<Vertex> generate_cursor_vertices();
static std::vector<uint32_t> generate_cursor_indices();

namespace mg1
{
  CursorComponent::CursorComponent(entt::entity id, CursorType type, glm::vec3 position) :
      IComponent(id), m_previous_position{ position }
  {
    std::string name;
    switch (type)
    {
    case CursorType::Object:
      name = ObjectLabel::object_cursor;
      break;
    case CursorType::Mouse:
      name = ObjectLabel::mouse_cursor;
      break;
    }

    m_info = std::make_shared<CursorInfo>(get_id(), name, type, position);

    ObjectAddedEvent e{ m_info.get() };
    post_event(e);
  }

  std::tuple<std::vector<Vertex>, std::vector<uint32_t>> CursorComponent::construct()
  {
    auto vertices = generate_cursor_vertices();
    auto indices  = generate_cursor_indices();

    return { vertices, indices };
  }

  void CursorComponent::update()
  {
    switch (m_info->m_type)
    {
    case CursorType::Object:
      break;
    case CursorType::Mouse:
      update_mouse();
      break;
    }
  }

  void CursorComponent::update_when_mouse_pressed()
  {
    if (EspInput::is_mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT)) { update(); }
  }

  void CursorComponent::update_mouse()
  {
    auto camera         = CadRenderer::get_camera();
    m_previous_position = get_position();
    auto new_pos        = Math::intersect_vector_plane(camera->get_position(), Math::s_mouse_ray, scene_plane);
    TransformManager::set_translation(m_id, new_pos);
    m_info->m_position = get_position();
  }

  void CursorComponent::handle_event(MouseMovedEvent& event, float dt, RotationAxis rotation_axis)
  {
    if (EspInput::is_mouse_button_pressed(GLFW_MOUSE_BUTTON_RIGHT))
    {
      bool rotated = false;

      switch (rotation_axis)
      {
      case RotationOX:
      {
        TransformManager::rotate(m_id, dt * event.get_dy(), { 1, 0, 0 });
        rotated = true;
        break;
      }
      case RotationOY:
      {
        TransformManager::rotate(m_id, dt * event.get_dx(), { 0, 1, 0 });
        rotated = true;
        break;
      }
      case RotationOZ:
      {
        TransformManager::rotate(m_id, dt * (event.get_dx() + event.get_dy()) / 2, { 0, 0, 1 });
        rotated = true;
        break;
      }
      default:
      {
        break;
      }
      }

      if (rotated)
      {
        CursorRotChangedEvent rot_event{ m_info->m_type, TransformManager::get_rotation(m_id) };
        post_event(rot_event);
      }
    }
  }

  void CursorComponent::handle_event(MouseScrolledEvent& event, ScaleAxis scale_axis)
  {
    float offset_y     = event.get_offset_y();
    float scale_factor = offset_y > 0 ? 1.05f : .95f;

    bool scaled = false;

    switch (scale_axis)
    {
    case Scale:
    {
      TransformManager::scale(m_id, scale_factor);
      scaled = true;
      break;
    }
    case ScaleOX:
    {
      TransformManager::scale_ox(m_id, scale_factor);
      scaled = true;
      break;
    }
    case ScaleOY:
    {
      TransformManager::scale_oy(m_id, scale_factor);
      scaled = true;
      break;
    }
    case ScaleOZ:
    {
      TransformManager::scale_oz(m_id, scale_factor);
      scaled = true;
      break;
    }
    default:
    {
      break;
    }
    }

    if (scaled)
    {
      CursorScaleChangedEvent scale_event{ m_info->m_type, TransformManager::get_scale(m_id) };
      post_event(scale_event);
    }
  }
} // namespace mg1

static void generate_cursor(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {}

static std::vector<Vertex> generate_cursor_vertices()
{
  std::vector<Vertex> vertices{};

  // OX strip
  vertices.push_back({ { 0, 0, 0 }, cursor_color_ox });
  vertices.push_back({ { cursor_size, 0, 0 }, cursor_color_ox });
  // OZ strip
  vertices.push_back({ { 0, 0, 0 }, cursor_color_oz });
  vertices.push_back({ { 0, 0, -cursor_size }, cursor_color_oz });
  // OY strip
  vertices.push_back({ { 0, 0, 0 }, cursor_color_oy });
  vertices.push_back({ { 0, cursor_size, 0 }, cursor_color_oy });

  return vertices;
}

static std::vector<uint32_t> generate_cursor_indices()
{
  std::vector<uint32_t> indices = { 0, 1, 0, 2, 3, 2, 4, 5, 4 };

  return indices;
}