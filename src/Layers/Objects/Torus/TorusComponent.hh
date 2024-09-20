#ifndef ESPERT_SANDBOX_TORUSCOMPONENT_HH
#define ESPERT_SANDBOX_TORUSCOMPONENT_HH

#include "Espert.hh"
#include "Events/Gui/GuiEvents.hh"
#include "Events/Objects/ObjectEvents.hh"
#include "Interfaces/Interfaces.hh"
#include "TorusInfo.hh"
#include "Utils/Constants.hh"

using namespace esp;

namespace mg1
{
  class TorusComponent : public IComponent, public IEventable
  {
   private:
    std::shared_ptr<TorusInfo> m_info;

   public:
    TorusComponent(entt::entity id,
                   float R           = TorusInit::S_R,
                   float r           = TorusInit::S_r,
                   int density_theta = TorusInit::S_DENSITY_THETA,
                   int density_phi   = TorusInit::S_DENSITY_PHI);
    ~TorusComponent() = default;

    std::tuple<std::vector<Vertex>, std::vector<uint32_t>> reconstruct();

    inline TorusInfo* get_info() { return m_info.get(); }

    void handle_event(CursorPosChangedEvent& event);
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_TORUSCOMPONENT_HH
