#ifndef ESPERT_SANDBOX_GUITOGGLEBUTTONCLICKEDEVENT_HH
#define ESPERT_SANDBOX_GUITOGGLEBUTTONCLICKEDEVENT_HH

#include "GuiEvent.hh"

namespace mg1
{
  enum ToggleState
  {
    Off,
    On
  };

  class GuiToggleButtonClickedEvent : public GuiEvent
  {
   private:
    ToggleState m_state;

   public:
    GuiToggleButtonClickedEvent(const std::string& label, ToggleState state) : GuiEvent(label), m_state{ state } {}

    inline ToggleState get_state() { return m_state; }
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_GUITOGGLEBUTTONCLICKEDEVENT_HH
