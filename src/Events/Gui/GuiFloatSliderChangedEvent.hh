#ifndef ESPERT_SANDBOX_GUIFLOATSLIDERCHANGEDEVENT_HH
#define ESPERT_SANDBOX_GUIFLOATSLIDERCHANGEDEVENT_HH

#include "GuiEvent.hh"

namespace mg1
{
  class GuiFloatSliderChangedEvent : public GuiEvent
  {
   private:
    float m_value;

   public:
    GuiFloatSliderChangedEvent(const std::string& label, float value) : GuiEvent(label), m_value{ value } {}

    inline float get_value() { return m_value; }
  };

} // namespace mg1

#endif // ESPERT_SANDBOX_GUIFLOATSLIDERCHANGEDEVENT_HH
