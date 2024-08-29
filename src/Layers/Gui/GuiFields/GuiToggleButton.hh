#ifndef ESPERT_SANDBOX_GUITOGGLEBUTTON_HH
#define ESPERT_SANDBOX_GUITOGGLEBUTTON_HH

#include "GuiButton.hh"

namespace mg1
{
  class GuiToggleButton : public GuiButton
  {
   public:
    GuiToggleButton(const std::string& label, glm::vec2 size = { 0, 0 }) : GuiButton(label, size) {}

    inline void render() override
    {
      if (clicked()) { ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]); }
      else { ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_Button]); }

      bool changed = false;
      if (m_max_width) { m_size.x = ImGui::GetContentRegionAvail().x; }
      if (ImGui::Button(m_label.c_str(), ImVec2(m_size.x, m_size.y)))
      {
        m_clicked = !m_clicked;
        changed   = true;
      }

      ImGui::PopStyleColor(1);

      if (changed) { create_and_post_event(); }
    }

    inline void create_and_post_event() override
    {
      GuiToggleButtonClickedEvent event{ m_label, m_clicked ? ToggleState::On : ToggleState::Off };
      post_event(event);
    }
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_GUITOGGLEBUTTON_HH
