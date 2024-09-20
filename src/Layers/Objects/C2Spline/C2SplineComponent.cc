#include "C2SplineComponent.hh"
#include "Layers/Objects/ObjectFactory.hh"

namespace mg1
{
  C2SplineComponent::C2SplineComponent(entt::entity id, std::vector<PointComponent> control_points) :
      C0SplineComponent(id)
  {
    sort_control_points(control_points);

    m_info = std::make_shared<C2SplineInfo>(get_id(),
                                            "C2 spline " + std::to_string(get_id()),
                                            create_point_infos(control_points));

    m_control_points           = create_control_points(control_points);
    m_bernstein_control_points = create_bernstein_control_points();

    ObjectAddedEvent e{ m_info.get() };
    post_event(e);
  }

  std::tuple<std::vector<Vertex>, std::vector<uint32_t>> C2SplineComponent::reconstruct()
  {
    if (m_spline_base == SplineBase::Bernstein)
    {
      for (int i = 0; i < m_bernstein_control_points.size(); i++)
      {
        auto& bernstein_point = ObjectFactory::get_control_point(m_bernstein_control_points[i]);
        if (bernstein_point.moved()) { update_control_points_positions(i); }
      }
    }

    auto vertices = create_bernstein_vertices();

    if (m_spline_base == SplineBase::Bernstein)
    {
      for (int i = 0; i < m_bernstein_control_points.size(); ++i)
      {
        auto& bernstein_point = ObjectFactory::get_control_point(m_bernstein_control_points[i]);
        TransformManager::set_translation(bernstein_point.get_id(), vertices[i + 2].m_position);
      }
    }

    m_info->m_dirty = false;

    return { vertices, get_spline_indices(vertices.size()) };
  }

  std::tuple<std::vector<Vertex>, std::vector<uint32_t>> C2SplineComponent::reconstruct_base()
  {
    return C0SplineComponent::reconstruct();
  }

  void C2SplineComponent::remove()
  {
    clear_bernstein_control_points();
    ObjectFactory::remove_object(*this);
  }

  void C2SplineComponent::handle_spline_base()
  {
    bool bernstein_empty = m_bernstein_control_points.empty();

    if (m_spline_base == SplineBase::Bernstein && bernstein_empty)
    {
      m_bernstein_control_points = create_bernstein_control_points();
    }
    if (m_spline_base == SplineBase::BSpline && !bernstein_empty) { clear_bernstein_control_points(); }
  }

  void C2SplineComponent::push_back(mg1::PointComponent& point)
  {
    C0SplineComponent::push_back(point);

    recreate_bernstein_control_points();
  }

  void C2SplineComponent::set_dirty_flag()
  {
    C0SplineComponent::set_dirty_flag();

    if (!m_info->m_dirty)
    {
      for (auto& id : m_bernstein_control_points)
      {
        auto& bernstein_point = ObjectFactory::get_control_point(id);
        if (bernstein_point.moved())
        {
          m_info->m_dirty = true;
          return;
        }
      }
    }
  }

  void C2SplineComponent::handle_event(ObjectRemovedEvent& event)
  {
    auto prev_size = m_control_points.size();

    C0SplineComponent::handle_event(event);

    if (prev_size != m_control_points.size()) // control point removed
    {
      recreate_bernstein_control_points();
    }
  }

  void C2SplineComponent::handle_event(GuiCheckboxChangedEvent& event) { C0SplineComponent::handle_event(event); }

  void C2SplineComponent::handle_event(CursorRotChangedEvent& event) { C0SplineComponent::handle_event(event); }

  void C2SplineComponent::handle_event(CursorScaleChangedEvent& event) { C0SplineComponent::handle_event(event); }

  void C2SplineComponent::handle_event(GuiInputIntChangedEvent& event)
  {
    if (m_info->selected())
    {
      m_spline_base   = (SplineBase)event.get_value();
      m_info->m_dirty = true;
    }
  }

  void C2SplineComponent::update_control_points_positions(int bernstein_point_idx)
  {
    auto& bernstein_point = ObjectFactory::get_control_point(m_bernstein_control_points[bernstein_point_idx]);
    bernstein_point_idx += 2;
    auto pos_diff         = bernstein_point.get_delta_position();
    auto bezier_point_idx = (bernstein_point_idx + 4) / 3;

    auto& p0 = ObjectFactory::get_control_point(m_control_points[bezier_point_idx]);
    auto& p1 = ObjectFactory::get_control_point(m_control_points[bezier_point_idx - 1]);
    auto& p2 = ObjectFactory::get_control_point(m_control_points[bezier_point_idx + 1]);

    auto p0_pos = p0.get_position(); // closest
    auto p1_pos = p1.get_position(); // one before
    auto p2_pos = p2.get_position(); // one after

    auto far_point = glm::vec3{};

    switch (bernstein_point_idx % 3)
    {
    case 0:
      far_point = (p1_pos + p2_pos) / 2.f;
      break;
    case 1:
      far_point = p2_pos;
      break;
    case 2:
      far_point = p1_pos;
      break;
    }

    auto scale = glm::length(p0_pos - far_point) / glm::length(-bernstein_point.get_position() + far_point);
    auto diff  = scale * pos_diff;
    TransformManager::translate(p0.get_id(), diff);
  }

  std::vector<Vertex> C2SplineComponent::create_bernstein_vertices()
  {
    std::vector<Vertex> vertices{};

    if (m_control_points.size() <= 4)
    {
      for (auto& point : m_control_points)
      {
        vertices.emplace_back(ObjectFactory::get_control_point(point).get_position());
      }

      return vertices;
    }

    vertices.reserve((m_control_points.size() - 4) * 3 + 4);

    auto& cp0 = ObjectFactory::get_control_point(m_control_points[0]);
    auto& cp1 = ObjectFactory::get_control_point(m_control_points[1]);
    auto& cp2 = ObjectFactory::get_control_point(m_control_points[2]);

    glm::vec3 e = cp0.get_position();
    glm::vec3 f = cp1.get_position();
    glm::vec3 g = 1.f / 3.f * cp1.get_position() + 2.f / 3.f * cp2.get_position();

    vertices.emplace_back(e);
    vertices.emplace_back(f);
    vertices.emplace_back(g);

    for (auto i = 2; i < m_control_points.size() - 2; i++)
    {
      auto& cp_i  = ObjectFactory::get_control_point(m_control_points[i]);
      auto& cp_ii = ObjectFactory::get_control_point(m_control_points[i + 1]);

      f = 2.f / 3.f * cp_i.get_position() + 1.f / 3.f * cp_ii.get_position();
      e = (f + g) / 2.f;
      g = 1.f / 3.f * cp_i.get_position() + 2.f / 3.f * cp_ii.get_position();

      vertices.emplace_back(e);
      vertices.emplace_back(f);
      if (i < m_control_points.size() - 3) { vertices.emplace_back(g); }
    }

    g = ObjectFactory::get_control_point(m_control_points[m_control_points.size() - 2]).get_position();
    e = ObjectFactory::get_control_point(m_control_points[m_control_points.size() - 1]).get_position();

    vertices.emplace_back(g);
    vertices.emplace_back(e);

    return std::move(vertices);
  }

  void C2SplineComponent::clear_bernstein_control_points()
  {
    for (auto& id : m_bernstein_control_points)
    {
      auto& bernstein_point = ObjectFactory::get_control_point(id);
      ObjectFactory::remove_object(bernstein_point);
    }
    m_bernstein_control_points.clear();
  }

  void C2SplineComponent::recreate_bernstein_control_points()
  {
    clear_bernstein_control_points();
    m_bernstein_control_points = create_bernstein_control_points();

    m_info->m_dirty = true;
  }

  std::vector<uint32_t> C2SplineComponent::create_bernstein_control_points()
  {
    std::vector<uint32_t> bernstein_control_points{};

    auto vertices      = create_bernstein_vertices();
    auto vertices_size = vertices.size();

    if (vertices_size <= 4) { return std::move(bernstein_control_points); }

    bernstein_control_points.reserve(vertices_size - 4);
    for (auto i = 2; i < vertices_size - 2; i++)
    {
      bernstein_control_points.push_back(mg1::ObjectFactory::create_bernstein_point(vertices[i].m_position).get_id());
    }

    return std::move(bernstein_control_points);
  }
} // namespace mg1