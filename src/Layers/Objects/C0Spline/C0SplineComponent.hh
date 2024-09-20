#ifndef ESPERT_SANDBOX_C0SPLINECOMPONENT_HH
#define ESPERT_SANDBOX_C0SPLINECOMPONENT_HH

#include "C0SplineInfo.hh"
#include "Espert.hh"
#include "Interfaces/Interfaces.hh"
#include "Layers/Objects/Point/PointComponent.hh"

using namespace esp;

namespace mg1
{
  class C0SplineComponent : public IComponent, public IEventable
  {
   protected:
    std::shared_ptr<C0SplineInfo> m_info;

    std::vector<uint32_t> m_control_points{};

   public:
    C0SplineComponent(entt::entity id, std::vector<PointComponent> control_points);
    C0SplineComponent(entt::entity id);
    ~C0SplineComponent() = default;

    virtual std::tuple<std::vector<Vertex>, std::vector<uint32_t>> reconstruct();
    virtual std::vector<uint32_t> get_spline_indices(uint32_t vertex_count);

    virtual void push_back(PointComponent& point);
    virtual void set_dirty_flag();

    inline C0SplineInfo* get_info() { return m_info.get(); }
    inline bool display_control_line() const { return m_info->display_control_line(); }

    virtual void handle_event(ObjectRemovedEvent& event);
    virtual void handle_event(GuiCheckboxChangedEvent& event);
    virtual void handle_event(CursorRotChangedEvent& event);
    virtual void handle_event(CursorScaleChangedEvent& event);

   protected:
    static void sort_control_points(std::vector<PointComponent>& control_points);
    static std::vector<uint32_t> create_control_points(std::vector<PointComponent>& control_points);
    static std::vector<PointInfo*> create_point_infos(std::vector<PointComponent>& control_points);
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_C0SPLINECOMPONENT_HH
