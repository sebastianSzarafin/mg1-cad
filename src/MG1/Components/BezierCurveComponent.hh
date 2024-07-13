#ifndef ESPERT_SANDBOX_BEZIERCURVECOMPONENT_HH
#define ESPERT_SANDBOX_BEZIERCURVECOMPONENT_HH

#include "Espert.hh"
#include "MG1/Common/ObjectInfo.hh"
#include "MG1/Interfaces/Interfaces.hh"
#include "PointComponent.hh"

using namespace esp;

namespace mg1
{
  class BezierCurveComponent : public IComponent, public IEventable
  {
   private:
    std::shared_ptr<BezierCurveInfo> m_info;

    // TODO: m_control_points list for improving performance when using reconstruct()

   public:
    BezierCurveComponent(uint32_t id, Scene* scene, std::vector<PointComponent> control_points);
    ~BezierCurveComponent() = default;

    std::tuple<std::vector<Vertex>, std::vector<uint32_t>> reconstruct();

    inline BezierCurveInfo* get_info() { return m_info.get(); }

    void handle_event(ObjectAddedEvent& event);
    void handle_event(ObjectRemovedEvent& event);
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_BEZIERCURVECOMPONENT_HH
