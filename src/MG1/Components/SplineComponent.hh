#ifndef ESPERT_SANDBOX_SPLINECOMPONENT_HH
#define ESPERT_SANDBOX_SPLINECOMPONENT_HH

#include "Espert.hh"
#include "MG1/Common/ObjectInfo.hh"
#include "MG1/Interfaces/Interfaces.hh"
#include "PointComponent.hh"

using namespace esp;

namespace mg1
{
  class SplineComponent : public IComponent, public IEventable
  {
   protected:
    std::shared_ptr<SplineInfo> m_info;

    std::vector<PointComponent> m_control_points{};

    bool m_display_control_line{ false };

   public:
    SplineComponent(uint32_t id, Scene* scene, std::vector<PointComponent> control_points);
    SplineComponent(uint32_t id, Scene* scene);
    ~SplineComponent() = default;

    virtual std::tuple<std::vector<Vertex>, std::vector<uint32_t>> reconstruct();

    virtual void push_back(PointComponent& point);

    inline SplineInfo* get_info() { return m_info.get(); }
    inline bool display_control_line() const { return m_display_control_line; }

    virtual void handle_event(ObjectRemovedEvent& event);
    virtual void handle_event(GuiCheckboxChangedEvent& event);

   protected:
    static std::vector<PointInfo*> create_point_infos(std::vector<PointComponent>& control_points);
    static std::vector<uint32_t> get_spline_indices(uint32_t vertex_count);
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_SPLINECOMPONENT_HH
