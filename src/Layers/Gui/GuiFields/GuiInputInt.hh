#ifndef ESPERT_SANDBOX_GUIINPUTINT_HH
#define ESPERT_SANDBOX_GUIINPUTINT_HH

#include "Events/Gui/GuiInputIntChangedEvent.hh"
#include "GuiField.hh"

namespace mg1
{
  class GuiInputInt : public GuiField<int>
  {
   private:
    int m_step;
    int m_step_fast;

    int m_min_value{ 1 };
    int m_max_value{ std::numeric_limits<int>::max() };

   public:
    GuiInputInt(const std::string& label, int value, int step = 1, int step_fast = 100) :
        GuiField(label, value), m_step{ step }, m_step_fast{ step_fast }
    {
    }

    inline void set_min_value(int value) { m_min_value = value; }

    inline void render() override
    {
      ImGui::InputInt(m_label.c_str(), &m_value, m_step, m_step_fast);
      m_value = std::clamp(m_value, m_min_value, m_max_value);

      if (changed()) { create_and_post_event(); }
    }

    inline void create_and_post_event() override
    {
      GuiInputIntChangedEvent e{ m_label, m_value };
      post_event(e);
    }
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_GUIINPUTINT_HH
