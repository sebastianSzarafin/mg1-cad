#ifndef ESPERT_SANDBOX_GUIRADIOBUTTONS_HH
#define ESPERT_SANDBOX_GUIRADIOBUTTONS_HH

#include <memory>

#include "GuiField.hh"
#include "MG1/Events/Gui/GuiInputIntChangedEvent.hh"

namespace mg1
{
  class GuiRadioButtons : public GuiField<int>
  {
   private:
    std::vector<std::string> m_labels;
    int m_prev_value;

   public:
    GuiRadioButtons(const std::string& label, int value, std::vector<std::string> labels) :
        GuiField(label, value), m_labels{ labels }, m_prev_value{ value }
    {
    }

    inline void render() override
    {
      for (int i = 0; i < m_labels.size(); i++)
      {
        ImGui::RadioButton(m_labels[i].c_str(), &m_value, i);
        if (i < m_labels.size() - 1) { ImGui::SameLine(); }

        if (changed()) { create_and_post_event(); }
      }
    }

    inline void create_and_post_event() override
    {
      GuiInputIntChangedEvent e{ m_label, m_value };
      post_event(e);
    }

    inline bool changed() override
    {
      if (m_prev_value != m_value)
      {
        m_prev_value = m_value;
        return true;
      }
      return false;
    }
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_GUIRADIOBUTTONS_HH
