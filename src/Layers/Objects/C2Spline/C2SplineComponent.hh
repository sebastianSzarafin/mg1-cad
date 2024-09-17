#ifndef ESPERT_SANDBOX_C2SPLINECOMPONENT_HH
#define ESPERT_SANDBOX_C2SPLINECOMPONENT_HH

#include "C2SplineInfo.hh"
#include "Layers/Objects/C0Spline/C0SplineComponent.hh"

namespace mg1
{
  class C2SplineComponent : public C0SplineComponent
  {
   private:
    SplineBase m_spline_base{ Bernstein };

    std::vector<uint32_t> m_bernstein_control_points{};

   public:
    C2SplineComponent(uint32_t id, std::vector<PointComponent> control_points);
    ~C2SplineComponent() = default;

    std::tuple<std::vector<Vertex>, std::vector<uint32_t>> reconstruct() override;
    std::tuple<std::vector<Vertex>, std::vector<uint32_t>> reconstruct_base();

    void remove();
    void handle_spline_base();
    void push_back(PointComponent& point) override;
    void set_dirty_flag() override;

    inline SplineBase get_spline_base() const { return m_spline_base; }

    void handle_event(ObjectRemovedEvent& event) override;
    void handle_event(GuiCheckboxChangedEvent& event) override;
    void handle_event(CursorRotChangedEvent& event) override;
    void handle_event(CursorScaleChangedEvent& event) override;
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
