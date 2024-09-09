#ifndef ESPERT_SANDBOX_C2INTERPOLATIONSPLINECOMPONENT_HH
#define ESPERT_SANDBOX_C2INTERPOLATIONSPLINECOMPONENT_HH

#include "Layers/Objects/C0Spline/C0SplineComponent.hh"

namespace mg1
{
  struct C2InterpolationSplineUbo
  {
    uint32_t m_display_control_line;
    SplineBase m_spline_base{ SplineBase::BSpline }; // TODO: remove
    alignas(16) std::array<glm::vec4, ObjectConstants::max_bezier_points> m_bezier_points;
  };

  class C2InterpolationSplineComponent : public C0SplineComponent
  {
   private:
    std::vector<float> m_control_points_diff{};
    std::vector<glm::vec3> m_control_points_pow{};

   public:
    C2InterpolationSplineComponent(uint32_t id, Scene* scene, std::vector<PointComponent> control_points);
    ~C2InterpolationSplineComponent() = default;

    std::tuple<std::vector<Vertex>, std::vector<uint32_t>> reconstruct() override;
    std::vector<uint32_t> get_spline_indices(uint32_t vertex_count) override;

    C2InterpolationSplineUbo get_ubo();

    void handle_event(ObjectRemovedEvent& event) override;
    void handle_event(GuiCheckboxChangedEvent& event) override;
    void handle_event(CursorRotChangedEvent& event) override;
    void handle_event(CursorScaleChangedEvent& event) override;

   private:
    std::vector<Vertex> create_bernstein_vertices();

    std::vector<glm::vec3> solve_tridiagonal();
    glm::vec3 equation_result(uint32_t point_idx);
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_C2INTERPOLATIONSPLINECOMPONENT_HH
