#ifndef ESPERT_SANDBOX_SPLINECOMPONENT_HH
#define ESPERT_SANDBOX_SPLINECOMPONENT_HH

#include "Common/ObjectInfo.hh"
#include "Espert.hh"
#include "Interfaces/Interfaces.hh"
#include "PointComponent.hh"
#include "Utils/Utils.hh"

using namespace esp;

namespace mg1
{
  struct SplineColorUbo
  {
    alignas(16) const glm::vec3 control_line_color = ObjectConstants::bernstein_point_color;
    alignas(16) glm::vec3 m_spline_color;
  };

  class SplineComponent : public IComponent, public IEventable
  {
   protected:
    std::shared_ptr<SplineInfo> m_info;

    std::vector<uint32_t> m_control_points{};

    bool m_display_control_line{ false };

   public:
    SplineComponent(uint32_t id, Scene* scene, std::vector<PointComponent> control_points);
    SplineComponent(uint32_t id, Scene* scene);
    ~SplineComponent() = default;

    virtual std::tuple<std::vector<Vertex>, std::vector<uint32_t>> reconstruct();
    virtual std::vector<uint32_t> get_spline_indices(uint32_t vertex_count);

    virtual void push_back(PointComponent& point);
    virtual void set_dirty_flag();

    inline SplineInfo* get_info() { return m_info.get(); }
    inline bool display_control_line() const { return m_display_control_line; }

    virtual void handle_event(ObjectRemovedEvent& event);
    virtual void handle_event(GuiCheckboxChangedEvent& event);

   protected:
    static void sort_control_points(std::vector<PointComponent>& control_points);
    static std::vector<uint32_t> create_control_points(std::vector<PointComponent>& control_points);
    static std::vector<PointInfo*> create_point_infos(std::vector<PointComponent>& control_points);

    PointComponent& get_control_point(uint32_t id);
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_SPLINECOMPONENT_HH
