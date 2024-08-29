#ifndef ESPERT_SANDBOX_GUICHECKBOX_HH
#define ESPERT_SANDBOX_GUICHECKBOX_HH

#include "Events/Gui/GuiCheckboxChangedEvent.hh"
#include "GuiField.hh"

namespace mg1
{
  class GuiCheckbox : public GuiField<bool>
  {
   public:
    GuiCheckbox(const std::string& label, bool value) : GuiField(label, value) {}

    inline void render() override
    {
      ImGui::Checkbox(m_label.c_str(), &m_value);

      if (changed()) { create_and_post_event(); }
    }

    inline void create_and_post_event() override
    {
      GuiCheckboxChangedEvent e{ m_label, m_value };
      post_event(e);
    }

    inline bool get_value() { return m_value; }
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_GUICHECKBOX_HH
