#ifndef ESPERT_SANDBOX_C2SPLINECOMPONENT_HH
#define ESPERT_SANDBOX_C2SPLINECOMPONENT_HH

#include "SplineComponent.hh"

namespace mg1
{
  class C2SplineComponent : public SplineComponent
  {
   private:
    SplineBase m_spline_base{ Bernstein };

   public:
    C2SplineComponent(uint32_t id, Scene* scene, std::vector<PointComponent> control_points);
    ~C2SplineComponent() = default;

    std::tuple<std::vector<Vertex>, std::vector<uint32_t>> reconstruct() override;

    inline SplineBase get_spline_base() const { return m_spline_base; }

    void handle_event(ObjectAddedEvent& event);
    void handle_event(ObjectRemovedEvent& event);
    void handle_event(GuiCheckboxChangedEvent& event);
    void handle_event(GuiInputIntChangedEvent& event);
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_C2SPLINECOMPONENT_HH
