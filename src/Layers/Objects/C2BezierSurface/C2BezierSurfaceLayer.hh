#ifndef ESPERT_SANDBOX_C2BEZIERSURFACELAYER_HH
#define ESPERT_SANDBOX_C2BEZIERSURFACELAYER_HH

#include "Espert.hh"
#include "Events/Gui/GuiEvents.hh"
#include "Events/Objects/ObjectEvents.hh"

using namespace esp;

namespace mg1
{
  class C2BezierSurfaceLayer : public Layer
  {
   private:
    Scene* m_scene;

    bool m_set_cursor_pos_action_selected{ false };

   public:
    C2BezierSurfaceLayer(Scene* scene);

    virtual void pre_update(float dt) override;
    virtual void update(float dt) override;
    virtual void post_update(float dt) override;
    virtual void handle_event(Event& event, float dt) override;

   private:
    bool gui_button_clicked_event_handler(GuiSurfacePopupModalCreateButtonClickedEvent& event);
    bool gui_selectable_changed_event_handler(GuiSelectableChangedEvent& event);
    bool gui_checkbox_changed_event_handler(GuiCheckboxChangedEvent& event);
    bool cursor_rot_changed_event_handler(CursorRotChangedEvent& event);
    bool cursor_scale_changed_event_handler(CursorScaleChangedEvent& event);

    glm::vec3 get_cursor_pos();
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_C2BEZIERSURFACELAYER_HH
