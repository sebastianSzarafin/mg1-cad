#ifndef ESPERT_SANDBOX_C0BEZIERSURFACEINFO_HH
#define ESPERT_SANDBOX_C0BEZIERSURFACEINFO_HH

#include "Layers/Gui/GuiFields/GuiCheckbox.hh"
#include "Layers/Objects/ObjectInfo.hh"
#include "Layers/Objects/Point/PointInfo.hh"

namespace mg1
{
  struct C0BezierSurfaceInfo : public ObjectInfo
  {
   private:
    std::shared_ptr<GuiCheckbox> m_control_line_checkbox;

   public:
    std::vector<PointInfo*> m_control_points;

    int m_tess_u{ 4 }; // segment division (horizontal)
    int m_tess_v{ 4 }; // segment division (vertical)

    bool m_dirty{ false };

    C0BezierSurfaceInfo(uint32_t id, const std::string& name, std::vector<PointInfo*> control_points) :
        ObjectInfo(id, name), m_control_points{ control_points }
    {
      m_control_line_checkbox = std::make_shared<GuiCheckbox>(GuiLabel::control_line_checkbox, false);

      m_removeable = true;
    }

    inline void select() override
    {
      if (m_state == ObjectState::Default) { m_state = ObjectState::Selected; }
      for (auto point : m_control_points)
      {
        point->select();
      }
    }
    inline void unselect() override
    {
      if (m_state == ObjectState::Selected) { m_state = ObjectState::Default; }
      for (auto point : m_control_points)
      {
        point->unselect();
      }
    }

    inline void render() override
    {
      ImGui::SeparatorText("Info:");
      ImGui::Spacing();
      ImGui::InputText("Name", &m_name);
      ImGui::Spacing();
      ImGui::InputInt("Tesselation (u)", &m_tess_u);
      ImGui::InputInt("Tesselation (v)", &m_tess_v);
      if (ImGui::IsItemDeactivatedAfterEdit()) { m_dirty = true; }
      ImGui::Spacing();
      for (auto point : m_control_points)
      {
        ImGui::Text("%s", point->m_name.c_str());
      }
      ImGui::Spacing();
      m_control_line_checkbox->render();
      ImGui::Spacing();
    }
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_C0BEZIERSURFACEINFO_HH
