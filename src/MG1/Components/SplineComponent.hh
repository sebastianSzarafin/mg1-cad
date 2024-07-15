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
   private:
    std::shared_ptr<SplineInfo> m_info;

    bool m_display_control_line{ false };
    // TODO: m_control_points list for improving performance when using reconstruct()

   public:
    SplineComponent(uint32_t id, Scene* scene, std::vector<PointComponent> control_points);
    ~SplineComponent() = default;

    std::tuple<std::vector<Vertex>, std::vector<uint32_t>> reconstruct();

    void push_back(PointComponent& point);

    inline SplineInfo* get_info() { return m_info.get(); }
    inline bool display_control_line() { return m_display_control_line; }

    void handle_event(ObjectAddedEvent& event);
    void handle_event(ObjectRemovedEvent& event);
    void handle_event(GuiCheckboxChangedEvent& event);
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_SPLINECOMPONENT_HH
