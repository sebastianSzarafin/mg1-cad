#ifndef ESPERT_SANDBOX_POINTINFO_HH
#define ESPERT_SANDBOX_POINTINFO_HH

#include "Layers/Objects/ObjectInfo.hh"

namespace mg1
{
  struct PointInfo : public ObjectInfo
  {
   private:
    static int s_selected_index;

   public:
    float m_r;

    bool m_dirty{ false };

    int m_selected_index{ -1 };

    PointInfo(uint32_t id, const std::string& name) : ObjectInfo(id, name) { m_renameable = m_removeable = true; }

    inline void select() override
    {
      if (m_state == ObjectState::Default) { m_state = ObjectState::Selected; }
      m_selected_index = ++PointInfo::s_selected_index;
    }
    inline void unselect() override
    {
      if (m_state == ObjectState::Selected) { m_state = ObjectState::Default; }
      PointInfo::s_selected_index--;
      m_selected_index = -1;
    }

    inline void render() override
    {
      ImGui::SeparatorText("Info:");
      ImGui::Spacing();
      ImGui::InputText("Name", &m_name);
      ImGui::InputFloat("R", &m_r, 0.05f, 1.0f, "%.2f");
      if (ImGui::IsItemDeactivatedAfterEdit()) { m_dirty = true; }
      ImGui::Spacing();
    }
  };

  inline int PointInfo::s_selected_index = -1;
} // namespace mg1

#endif // ESPERT_SANDBOX_POINTINFO_HH
