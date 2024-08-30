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
  CursorComponent::CursorComponent(uint32_t id, CursorType type, glm::vec3 position) :
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

    m_info = std::make_shared<CursorInfo>(m_id, name, type, position);

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
    auto camera = Scene::get_current_camera();
    glm::vec3 ray_mouse =
        Math::cast_ray(Math::get_mouse_x_cs(), Math::get_mouse_y_cs(), camera->get_view(), camera->get_projection());
    m_previous_position = m_node->get_translation();
    m_node->set_translation(Math::intersect_vector_plane(camera->get_position(), ray_mouse, scene_plane));
    m_info->m_position = m_node->get_translation();
  }

  void CursorComponent::handle_event(MouseMovedEvent& event, float dt, RotationAxis rotation_axis)
  {
    if (EspInput::is_mouse_button_pressed(GLFW_MOUSE_BUTTON_RIGHT))
    {
      switch (rotation_axis)
      {
      case RotationOX:
      {
        m_node->rotate(dt * event.get_dy(), { 1, 0, 0 });
        break;
      }
      case RotationOY:
      {
        m_node->rotate(dt * event.get_dx(), { 0, 1, 0 });
        break;
      }
      case RotationOZ:
      {
        m_node->rotate(dt * (event.get_dx() + event.get_dy()) / 2, { 0, 0, 1 });
        break;
      }
      default:
      {
        break;
      }
      }
    }
  }

  void CursorComponent::handle_event(MouseScrolledEvent& event, ScaleAxis scale_axis)
  {
    float offset_y     = event.get_offset_y();
    float scale_factor = offset_y > 0 ? 1.05f : .95f;

    switch (scale_axis)
    {
    case Scale:
    {
      m_node->scale(scale_factor);
      break;
    }
    case ScaleOX:
    {
      m_node->scale_ox(scale_factor);
      break;
    }
    case ScaleOY:
    {
      m_node->scale_oy(scale_factor);
      break;
    }
    case ScaleOZ:
    {
      m_node->scale_oz(scale_factor);
      break;
    }
    default:
    {
      break;
    }
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