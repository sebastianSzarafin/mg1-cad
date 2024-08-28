#ifndef ESPERT_SANDBOX_GUIFLOATSLIDER_HH
#define ESPERT_SANDBOX_GUIFLOATSLIDER_HH

#include "GuiField.hh"

namespace mg1
{
  class GuiFloatSlider : public GuiField<float>
  {
   private:
    float m_min_value;
    float m_max_value;

    float m_prev_value;

    std::string m_format;

   public:
    GuiFloatSlider(const std::string& label,
                   float value,
                   float min_value           = -1,
                   float max_value           = 1,
                   const std::string& format = "%.2f") :
        GuiField(label, value), m_min_value{ min_value }, m_max_value{ max_value }, m_prev_value{ value },
        m_format{ format }
    {
    }

    inline void render() override
    {
      ImGui::SliderFloat(m_label.c_str(), &m_value, m_min_value, m_max_value, m_format.c_str());

      if (changed()) { create_and_post_event(); }
    }

    inline void create_and_post_event() override
    {
      GuiInputFloatChangedEvent e{ m_label, m_value };
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

#endif // ESPERT_SANDBOX_GUIFLOATSLIDER_HH
