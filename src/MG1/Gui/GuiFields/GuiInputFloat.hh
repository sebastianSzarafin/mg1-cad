#ifndef ESPERT_SANDBOX_GUIINPUTFLOAT_HH
#define ESPERT_SANDBOX_GUIINPUTFLOAT_HH

#include <memory>

#include "GuiField.hh"
#include "MG1/Gui/GuiEvents/GuiInputFloatChangedEvent.hh"

namespace mg1
{
  class GuiInputFloat : public GuiField<float>
  {
   private:
    float m_step;
    float m_step_fast;

    float m_min_value{ 0 };
    float m_max_value{ FLT_MAX };

    std::string m_format;

   public:
    GuiInputFloat(const std::string& label,
                  float value,
                  float step                = 0.f,
                  float step_fast           = 0.f,
                  const std::string& format = "%.2f") :
        GuiField(label, value), m_step{ step }, m_step_fast{ step_fast }, m_format{ format }
    {
    }

    inline void render() override
    {
      ImGui::InputFloat(m_label.c_str(), &m_value, m_step, m_step_fast, m_format.c_str());
      m_value = std::clamp(m_value, m_min_value, m_max_value);
    }

    inline std::shared_ptr<Event> create_event() override
    {
      return std::make_shared<GuiInputFloatChangedEvent>(m_label, m_value);
    }
  };

} // namespace mg1

#endif // ESPERT_SANDBOX_GUIINPUTFLOAT_HH
