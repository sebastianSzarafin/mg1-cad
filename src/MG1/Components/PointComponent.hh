#ifndef ESPERT_SANDBOX_POINTCOMPONENT_HH
#define ESPERT_SANDBOX_POINTCOMPONENT_HH

#include "Espert.hh"
#include "MG1/Common/InitInfo.hh"
#include "MG1/Common/ObjectInfo.hh"
#include "MG1/Events/Gui/GuiEvents.hh"
#include "MG1/Events/Object/ObjectEvents.hh"
#include "MG1/Interfaces/Interfaces.hh"
#include "MG1/Utils.hh"

using namespace esp;

namespace mg1
{
  class PointComponent : public IComponent, public IEventable
  {
   private:
    std::shared_ptr<PointInfo> m_info;

    bool m_clicked{ false };
    bool m_bernstein_point{ false };

    glm::vec3 m_delta_position{ 0.f };

   public:
    PointComponent(uint32_t id, bool bernstein_point, float r = PointInit::S_R);
    ~PointComponent() = default;

    std::tuple<std::vector<Vertex>, std::vector<uint32_t>> reconstruct();

    inline PointInfo* get_info() { return m_info.get(); }
    inline bool clicked() const { return m_clicked; }
    inline bool bernstein_point() const { return m_bernstein_point; }

    inline glm::vec3 get_position() const { return m_node->get_translation(); }
    inline glm::vec3 get_delta_position() const { return m_delta_position; }

    void handle_event(MouseButtonPressedEvent& event);
    void handle_event(CursorPosChangedEvent& event);
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_POINTCOMPONENT_HH
