#ifndef ESPERT_SANDBOX_C2SPLINELAYER_HH
#define ESPERT_SANDBOX_C2SPLINELAYER_HH

#include "Espert.hh"
#include "Events/Gui/GuiEvents.hh"
#include "Events/Objects/ObjectEvents.hh"

using namespace esp;

namespace mg1
{
  class C2SplineLayer : public Layer
  {
   private:
    Scene* m_scene;

   public:
    C2SplineLayer(Scene* scene);

    virtual void pre_update(float dt) override;
    virtual void update(float dt) override;
    virtual void post_update(float dt) override;
    virtual void handle_event(Event& event, float dt) override;

   private:
    bool gui_button_clicked_event_handler(GuiButtonClickedEvent& event);
    bool mouse_button_pressed_event_handler(MouseButtonPressedEvent& event);
    bool object_removed_event_handler(ObjectRemovedEvent& event);
    bool gui_input_int_field_changed_event_handler(GuiInputIntChangedEvent& event);
    bool cursor_rot_changed_event_handler(CursorRotChangedEvent& event);
    bool cursor_scale_changed_event_handler(CursorScaleChangedEvent& event);
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_C2SPLINELAYER_HH
