#include "C2InterpolationSplineComponent.hh"
#include "Layers/Objects/ObjectFactory.hh"

namespace mg1
{

  C2InterpolationSplineComponent::C2InterpolationSplineComponent(uint32_t id,
                                                                 Scene* scene,
                                                                 std::vector<PointComponent> control_points) :
      C0SplineComponent(id, scene)
  {
    sort_control_points(control_points);

    m_info = std::make_shared<C0SplineInfo>(m_id,
                                            "C2 interpolation spline " + std::to_string(m_id),
                                            create_point_infos(control_points));

    m_control_points = create_control_points(control_points);

    ObjectAddedEvent e{ m_info.get() };
    post_event(e);
  }

  std::tuple<std::vector<Vertex>, std::vector<uint32_t>> C2InterpolationSplineComponent::reconstruct()
  {
    auto vertices = create_bernstein_vertices();

    m_info->m_dirty = false;

    return { vertices, get_spline_indices(vertices.size()) };
  }

  std::tuple<std::vector<Vertex>, std::vector<uint32_t>> C2InterpolationSplineComponent::reconstruct_base()
  {
    return C0SplineComponent::reconstruct();
  }

  std::vector<uint32_t> C2InterpolationSplineComponent::get_spline_indices(uint32_t vertex_count)
  {
    if (vertex_count <= 4) { return { 0, 1, 2, 3 }; }

    std::vector<uint32_t> indices{};
    indices.reserve(vertex_count);
    for (auto i = 0; i < vertex_count; i++)
    {
      indices.push_back(i);
    }

    return indices;
  }

  void C2InterpolationSplineComponent::handle_event(ObjectRemovedEvent& event)
  {
    C0SplineComponent::handle_event(event);
  }

  void C2InterpolationSplineComponent::handle_event(GuiCheckboxChangedEvent& event)
  {
    C0SplineComponent::handle_event(event);
  }

  void C2InterpolationSplineComponent::handle_event(CursorRotChangedEvent& event)
  {
    C0SplineComponent::handle_event(event);
  }

  void C2InterpolationSplineComponent::handle_event(CursorScaleChangedEvent& event)
  {
    C0SplineComponent::handle_event(event);
  }

  std::vector<Vertex> C2InterpolationSplineComponent::create_bernstein_vertices()
  {
    auto cp_count = m_control_points.size();
    auto n        = cp_count - 1;

    std::vector<Vertex> vertices{};

    if (cp_count < 2) { return {}; }
    else if (cp_count == 2)
    {
      vertices.reserve(4);

      auto& cp0 = ObjectFactory::get_control_point(m_control_points[0]);
      auto& cp1 = ObjectFactory::get_control_point(m_control_points[1]);

      vertices.emplace_back(cp0.get_position());
      vertices.emplace_back(cp0.get_position());
      vertices.emplace_back(cp1.get_position());
      vertices.emplace_back(cp1.get_position());

      return std::move(vertices);
    }

    auto eps = .001f;
    m_control_points_diff.clear();
    m_control_points_diff.resize(n + 1);

    for (int i = 0; i < n; i++)
    {
      auto& cp_i  = ObjectFactory::get_control_point(m_control_points[i]);
      auto& cp_ii = ObjectFactory::get_control_point(m_control_points[i + 1]);

      auto dist                = glm::length(cp_ii.get_position() - cp_i.get_position());
      m_control_points_diff[i] = std::abs(dist) > eps ? dist : eps;
    }

    m_control_points_diff[n] = 1.f;

    auto c_params = solve_tridiagonal();

    m_control_points_pow.clear();
    m_control_points_pow.resize(4 * (n + 1));

    for (auto i = 0; i < n; i++)
    {
      auto& p_i = ObjectFactory::get_control_point(m_control_points[i]);

      m_control_points_pow[i * 4 + 0] = p_i.get_position();
      m_control_points_pow[i * 4 + 2] = c_params[i];
    }

    m_control_points_pow[n * 4] = ObjectFactory::get_control_point(m_control_points[n]).get_position();

    for (auto i = 1; i < n; i++)
    {
      m_control_points_pow[(i - 1) * 4 + 3] =
          (m_control_points_pow[i * 4 + 2] - m_control_points_pow[(i - 1) * 4 + 2]) /
          (3.f * m_control_points_diff[i - 1]);
    }

    for (auto i = 1; i < n; i++)
    {
      m_control_points_pow[(i - 1) * 4 + 1] =
          (m_control_points_pow[i * 4] - m_control_points_pow[(i - 1) * 4] -
           m_control_points_pow[(i - 1) * 4 + 2] * powf(m_control_points_diff[i - 1], 2.f) -
           m_control_points_pow[(i - 1) * 4 + 3] * powf(m_control_points_diff[i - 1], 3.f)) /
          m_control_points_diff[i - 1];
    }

    if (n > 1)
    {
      m_control_points_pow[(n - 1) * 4 + 1] = m_control_points_pow[(n - 2) * 4 + 1] +
          2.f * m_control_points_pow[(n - 2) * 4 + 2] * m_control_points_diff[n - 2] +
          3.f * m_control_points_pow[(n - 2) * 4 + 3] * powf(m_control_points_diff[n - 2], 2.f);
      m_control_points_pow[(n - 1) * 4 + 3] =
          (m_control_points_pow[n * 4] - m_control_points_pow[(n - 1) * 4] -
           m_control_points_pow[(n - 1) * 4 + 1] * m_control_points_diff[n - 1] -
           m_control_points_pow[(n - 1) * 4 + 2] * powf(m_control_points_diff[n - 1], 2.f)) /
          powf(m_control_points_diff[n - 1], 3.f);
    }

    //
    vertices.reserve(m_control_points_pow.size());
    for (auto i = 0; i < m_control_points_pow.size(); i += 4)
    {
      float dist = m_control_points_diff[i / 4];

      glm::mat4 bernstein_to_power(1.f,
                                   -3.f / dist,
                                   3.f / powf(dist, 2.f),
                                   -1.f / powf(dist, 3.f),
                                   0.f,
                                   3.f / dist,
                                   -6.f / powf(dist, 2.f),
                                   3.f / powf(dist, 3.f),
                                   0.f,
                                   0.f,
                                   3.f / powf(dist, 2.f),
                                   -3.f / powf(dist, 3.f),
                                   0.f,
                                   0.f,
                                   0.f,
                                   1.f / powf(dist, 3.f));

      bernstein_to_power = glm::transpose(bernstein_to_power);

      glm::mat4 power_to_bernstein = glm::inverse(bernstein_to_power);

      glm::vec4 v1, v2, v3;

      v1 = { m_control_points_pow[i + 0].x,
             m_control_points_pow[i + 1].x,
             m_control_points_pow[i + 2].x,
             m_control_points_pow[i + 3].x };
      v2 = { m_control_points_pow[i + 0].y,
             m_control_points_pow[i + 1].y,
             m_control_points_pow[i + 2].y,
             m_control_points_pow[i + 3].y };
      v3 = { m_control_points_pow[i + 0].z,
             m_control_points_pow[i + 1].z,
             m_control_points_pow[i + 2].z,
             m_control_points_pow[i + 3].z };

      v1 = v1 * power_to_bernstein;
      v2 = v2 * power_to_bernstein;
      v3 = v3 * power_to_bernstein;

      vertices.emplace_back(glm::vec3(v1.x, v2.x, v3.x));
      vertices.emplace_back(glm::vec3(v1.y, v2.y, v3.y));
      vertices.emplace_back(glm::vec3(v1.z, v2.z, v3.z));
      vertices.emplace_back(glm::vec3(v1.w, v2.w, v3.w));
    }
    //

    return std::move(vertices);
  }

  std::vector<glm::vec3> C2InterpolationSplineComponent::solve_tridiagonal()
  {
    auto cp_count = m_control_points.size();
    if (cp_count < 2) { return {}; }

    auto eq_count = cp_count - 2;
    std::vector<glm::vec3> c_params(cp_count);

    if (eq_count == 0) { return c_params; }

    std::vector<float> c_prim(1 > eq_count - 1 ? 1 : eq_count - 1);
    std::vector<glm::vec3> d_prim(eq_count);

    c_prim[0] = m_control_points_diff[1] / (2.f * (m_control_points_diff[0] + m_control_points_diff[1]));
    d_prim[0] = equation_result(0) / 2.f;

    for (auto i = 1; i < eq_count - 1; i++)
    {
      auto a = m_control_points_diff[i] / (m_control_points_diff[i] + m_control_points_diff[i + 1]);
      auto b = 2.f;
      auto c = 1.f - a;

      auto denominator = b - a * c_prim[i - 1];
      c_prim[i]        = c / denominator;
      d_prim[i]        = (equation_result(i) - a * d_prim[i - 1]) / denominator;
    }

    if (eq_count > 1)
    {
      auto a =
          m_control_points_diff[eq_count - 1] / (m_control_points_diff[eq_count - 1] + m_control_points_diff[eq_count]);
      auto b = 2.f;

      auto denominator     = b - a * c_prim[eq_count - 2];
      d_prim[eq_count - 1] = (equation_result(eq_count - 1) - a * d_prim[eq_count - 2]) / denominator;
    }

    c_params[cp_count - 2] = d_prim[eq_count - 1];

    for (auto i = cp_count - 3; i > 0; i--)
    {
      c_params[i] = d_prim[i - 1] - c_prim[i - 1] * c_params[i + 1];
    }

    return c_params;
  }

  glm::vec3 C2InterpolationSplineComponent::equation_result(uint32_t point_idx)
  {
    auto& p0 = ObjectFactory::get_control_point(m_control_points[point_idx]);
    auto& p1 = ObjectFactory::get_control_point(m_control_points[point_idx + 1]);
    auto& p2 = ObjectFactory::get_control_point(m_control_points[point_idx + 2]);

    return 3.f / (m_control_points_diff[point_idx] + m_control_points_diff[point_idx + 1]) *
        ((p2.get_position() - p1.get_position()) / m_control_points_diff[point_idx + 1] -
         (p1.get_position() - p0.get_position()) / m_control_points_diff[point_idx]);
  }
} // namespace mg1