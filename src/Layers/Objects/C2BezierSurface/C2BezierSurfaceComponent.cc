#include "C2BezierSurfaceComponent.hh"
#include "Layers/Objects/ObjectFactory.hh"

namespace mg1
{

  C2BezierSurfaceComponent::C2BezierSurfaceComponent(entt::entity id, SurfaceComponentParams data) :
      C0BezierSurfaceComponent(id)
  {
    m_type      = data.m_type;
    m_patches_u = data.m_segments_u;
    m_patches_v = data.m_segments_v;
    m_wrap_u    = m_type == SurfaceType::Cylinder;
    m_points_u  = s_patch_size + s_patch_offset * (m_patches_u - 1) - (m_wrap_u ? 1 : 0);
    m_points_v  = s_patch_size + s_patch_offset * (m_patches_v - 1);

    m_control_points = create_control_points(data);
    m_vertex_count   = generate_patches().size();

    m_info = std::make_shared<C0BezierSurfaceInfo>(get_id(),
                                                   "C2 bezier surface " + std::to_string(get_id()),
                                                   create_point_infos(m_control_points));

    m_surface_indices      = generate_surface_indices();
    m_control_line_indices = generate_control_line_indices();

    ObjectAddedEvent e{ m_info.get() };
    post_event(e);
  }

  void C2BezierSurfaceComponent::handle_event(GuiCheckboxChangedEvent& event)
  {
    C0BezierSurfaceComponent::handle_event(event);
  }

  void C2BezierSurfaceComponent::handle_event(CursorRotChangedEvent& event)
  {
    C0BezierSurfaceComponent::handle_event(event);
  }

  void C2BezierSurfaceComponent::handle_event(CursorScaleChangedEvent& event)
  {
    C0BezierSurfaceComponent::handle_event(event);
  }

  std::vector<uint32_t> C2BezierSurfaceComponent::create_control_points(SurfaceComponentParams data)
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
      auto n = 2 * glm::pi<float>() / m_points_u;
      for (auto v = 0; v < m_points_v; v++)
      {
        for (auto u = 0; u < m_points_u; u++)
        {
          auto z = v * data.m_height / (m_points_v - 1) - data.m_height / 2.f;

          auto p = glm::vec3(glm::cos(u * n), glm::sin(u * n), 0);

          p *= data.m_width;

          p.z = z;

          control_points.push_back(ObjectFactory::create_point(p).get_id());
        }
      }
    }

    return std::move(control_points);
  }
} // namespace mg1