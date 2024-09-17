#include "C0BezierSurfaceComponent.hh"
#include "Layers/Objects/ObjectFactory.hh"

namespace mg1
{
  C0BezierSurfaceComponent::C0BezierSurfaceComponent(int id, mg1::SurfaceComponentParams data) : IComponent(id)
  {
    m_type      = data.m_type;
    m_patches_u = data.m_segments_u;
    m_patches_v = data.m_segments_v;
    m_wrap_u    = m_type == SurfaceType::Cylinder;
    m_points_u  = s_patch_size + s_patch_offset * (m_patches_u - 1) - (m_wrap_u ? 1 : 0);
    m_points_v  = s_patch_size + s_patch_offset * (m_patches_v - 1);

    m_control_points = create_control_points(data);
    m_vertex_count   = generate_patches().size();

    m_info = std::make_shared<C0BezierSurfaceInfo>(m_id,
                                                   "C0 bezier surface " + std::to_string(m_id),
                                                   create_point_infos(m_control_points));

    m_surface_indices      = generate_surface_indices();
    m_control_line_indices = generate_control_line_indices();

    ObjectAddedEvent e{ m_info.get() };
    post_event(e);
  }

  C0BezierSurfaceComponent::C0BezierSurfaceComponent(int id) : IComponent(id) {}

  std::tuple<std::vector<Vertex>, std::vector<uint32_t>> C0BezierSurfaceComponent::reconstruct()
  {
    auto vertices = generate_patches();

    m_info->m_dirty = false;

    return { vertices, get_surface_indices() };
  }

  void C0BezierSurfaceComponent::set_dirty_flag()
  {
    for (auto& point : m_control_points)
    {
      if (ObjectFactory::get_control_point(point).moved())
      {
        m_info->m_dirty = true;
        return;
      }
    }
  }

  void C0BezierSurfaceComponent::translate(glm::vec3 position)
  {
    for (auto& id : m_control_points)
    {
      ObjectFactory::get_control_point(id).get_node()->translate(position);
    }
  }

  void C0BezierSurfaceComponent::remove()
  {
    for (auto& id : m_control_points)
    {
      auto& point = ObjectFactory::get_control_point(id);
      ObjectFactory::remove_object(point);
    }
    m_control_points.clear();

    ObjectFactory::remove_object(*this);
  }

  void C0BezierSurfaceComponent::handle_event(mg1::GuiCheckboxChangedEvent& event)
  {
    if (m_info->selected()) { m_info->m_dirty = true; }
  }

  void C0BezierSurfaceComponent::handle_event(CursorRotChangedEvent& event)
  {
    if (m_info->selected()) { m_info->m_dirty = true; }
  }

  void C0BezierSurfaceComponent::handle_event(CursorScaleChangedEvent& event)
  {
    if (m_info->selected()) { m_info->m_dirty = true; }
  }

  std::vector<uint32_t> C0BezierSurfaceComponent::create_control_points(SurfaceComponentParams data)
  {
    std::vector<uint32_t> control_points{};

    // create plane
    if (!m_wrap_u)
    {
      for (auto v = 0; v < m_points_v; v++)
      {
        for (auto u = 0; u < m_points_u; u++)
        {
          auto x = u * data.m_width / (m_points_u - 1) - data.m_width / 2.f;
          auto z = v * data.m_height / (m_points_v - 1) - data.m_height / 2.f;

          control_points.push_back(ObjectFactory::create_surface_point({ x, 0, z }).get_id());
        }
      }
    }

    // create cylinder
    if (m_wrap_u)
    {
      auto n = 2 * glm::pi<float>() / m_patches_u;
      for (auto v = 0; v < m_points_v; v++)
      {
        for (auto u = 0; u < m_patches_u; u++)
        {
          auto z = v * data.m_height / (m_points_v - 1) - data.m_height / 2.f;

          auto p1 = glm::vec3(glm::cos(u * n), glm::sin(u * n), 0);
          auto p4 = glm::vec3(glm::cos((u + 1) * n), glm::sin((u + 1) * n), 0);

          auto t1 = glm::vec3(-p1.y, p1.x, 0);
          auto t2 = glm::vec3(p4.y, -p4.x, 0);

          auto p2 = p1 + t1 * 4.f / 3.f * glm::tan(glm::pi<float>() / (2 * m_patches_u));
          auto p3 = p4 + t2 * 4.f / 3.f * glm::tan(glm::pi<float>() / (2 * m_patches_u));

          p1 *= data.m_width;
          p2 *= data.m_width;
          p3 *= data.m_width;
          // p4 *= data.m_width; (?)

          p1.z = z;
          p2.z = z;
          p3.z = z;
          // p4.z = z; (?)

          control_points.push_back(ObjectFactory::create_point(p1).get_id());
          control_points.push_back(ObjectFactory::create_point(p2).get_id());
          control_points.push_back(ObjectFactory::create_point(p3).get_id());
          // control_points.push_back(ObjectFactory::create_point(p4).get_id()); (?)
        }
      }
    }

    return std::move(control_points);
  }

  std::vector<PointInfo*> C0BezierSurfaceComponent::create_point_infos(std::vector<uint32_t>& control_points)
  {
    std::vector<PointInfo*> infos{};

    std::transform(control_points.begin(),
                   control_points.end(),
                   std::back_inserter(infos),
                   [](uint32_t id) { return ObjectFactory::get_control_point(id).get_info(); });

    return std::move(infos);
  }

  std::vector<Vertex> C0BezierSurfaceComponent::generate_patches()
  {
    std::vector<Vertex> vertices{};

    auto curr_u = 0, curr_v = 0;

    while (curr_u * patch_offset() + patch_size() <= m_points_u + (m_wrap_u ? (patch_size() - patch_offset()) : 0))
    {
      while (curr_v * patch_offset() + patch_size() <= m_points_v)
      {
        for (auto u = curr_u * patch_offset(); u < curr_u * patch_offset() + patch_size(); u++)
        {
          for (auto v = curr_v * patch_offset(); v < curr_v * patch_offset() + patch_size(); v++)
          {
            // auto curr_off_u = u - curr_u * s_patch_offset;
            // auto curr_off_v = v - curr_v * s_patch_offset;

            auto u_wrap = u % m_points_u;
            auto v_wrap = v % m_points_v;

            vertices.push_back(
                { ObjectFactory::get_control_point(m_control_points[u_wrap + v_wrap * m_points_u]).get_position() });
          }
        }

        curr_v++;
      }

      curr_v = 0;
      curr_u++;
    }

    return vertices;
  }

  std::vector<uint32_t> C0BezierSurfaceComponent::generate_surface_indices()
  {
    std::vector<uint32_t> indices{};
    indices.reserve(m_vertex_count);
    for (auto i = 0; i < m_vertex_count; i++)
    {
      indices.push_back(i);
    }

    return indices;
  }

  std::vector<uint32_t> C0BezierSurfaceComponent::generate_control_line_indices()
  {
    std::vector<uint32_t> control_line_indices{};
    control_line_indices.reserve(m_vertex_count * 2);

    // vertical lines
    for (auto i = 0; i < m_vertex_count; i++)
    {
      auto next = i + 1;
      if (next % patch_size() != 0)
      {
        control_line_indices.push_back(i);
        control_line_indices.push_back(next);
      }
    }

    // horizontal lines
    for (auto i = 0; i < m_vertex_count; i++)
    {
      auto next = i + patch_size();
      if ((i - 12) % (patch_size() * patch_size()) != 0)
      {
        control_line_indices.push_back(i);
        control_line_indices.push_back(next);
      }
      else { i += patch_size() - 1; }
    }

    return control_line_indices;
  }
} // namespace mg1