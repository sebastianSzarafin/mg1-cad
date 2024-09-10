#ifndef ESPERT_SANDBOX_POINTCOMPONENT_HH
#define ESPERT_SANDBOX_POINTCOMPONENT_HH

#include "Espert.hh"
#include "Events/Gui/GuiEvents.hh"
#include "Events/Objects/ObjectEvents.hh"
#include "Interfaces/Interfaces.hh"
#include "PointInfo.hh"
#include "Utils/Constants.hh"

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
    inline bool moved() const { return glm::length2(m_delta_position) > 0.f; }

    inline glm::vec3 get_position() const
    {
      auto model = m_node->get_model_mat();
      return { model[3][0], model[3][1], model[3][2] };
    }
    inline glm::vec3 get_delta_position() const { return m_delta_position; }

    void handle_event(MouseButtonPressedEvent& event);
    void handle_event(CursorPosChangedEvent& event);
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_POINTCOMPONENT_HH
