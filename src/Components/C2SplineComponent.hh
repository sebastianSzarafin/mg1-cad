#ifndef ESPERT_SANDBOX_C2SPLINECOMPONENT_HH
#define ESPERT_SANDBOX_C2SPLINECOMPONENT_HH

#include "SplineComponent.hh"

namespace mg1
{
  struct C2SplineUbo
  {
    uint32_t m_display_control_line;
    SplineBase m_spline_base;
    alignas(16) std::array<glm::vec4, ObjectConstants::max_bezier_points> m_bezier_points;
  };

  class C2SplineComponent : public SplineComponent
  {
   private:
    SplineBase m_spline_base{ Bernstein };

    std::vector<uint32_t> m_bernstein_control_points{};

   public:
    C2SplineComponent(uint32_t id, Scene* scene, std::vector<PointComponent> control_points);
    ~C2SplineComponent() = default;

    std::tuple<std::vector<Vertex>, std::vector<uint32_t>> reconstruct() override;

    void remove();
    void handle_spline_base();
    void push_back(PointComponent& point) override;
    void set_dirty_flag() override;

    C2SplineUbo get_ubo();

    void handle_event(ObjectRemovedEvent& event) override;
    void handle_event(GuiCheckboxChangedEvent& event) override;
    void handle_event(GuiInputIntChangedEvent& event);

   private:
    std::vector<Vertex> create_bernstein_vertices();
    std::vector<uint32_t> create_bernstein_control_points();

    void clear_bernstein_control_points();
    void recreate_bernstein_control_points();
    void update_control_points_positions(int bernstein_point_idx);
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_C2SPLINECOMPONENT_HH
