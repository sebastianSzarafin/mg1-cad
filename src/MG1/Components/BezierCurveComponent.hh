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

    std::vector<PointComponent> m_control_points;

   public:
    BezierCurveComponent(uint32_t id, std::vector<PointComponent> control_points);
    ~BezierCurveComponent() = default;

    std::tuple<std::vector<Vertex>, std::vector<uint32_t>> reconstruct(); // TODO: don't create fake model

    inline BezierCurveInfo* get_info() { return m_info.get(); }
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_BEZIERCURVECOMPONENT_HH
