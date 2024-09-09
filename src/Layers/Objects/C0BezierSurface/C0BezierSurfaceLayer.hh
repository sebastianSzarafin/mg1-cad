#ifndef ESPERT_SANDBOX_C0BEZIERSURFACELAYER_HH
#define ESPERT_SANDBOX_C0BEZIERSURFACELAYER_HH

#include "Espert.hh"
#include "Events/Gui/GuiEvents.hh"
#include "Events/Objects/ObjectEvents.hh"

using namespace esp;

namespace mg1
{
  class C0BezierSurfaceLayer : public Layer
  {
   private:
    Scene* m_scene;

   public:
    C0BezierSurfaceLayer(Scene* scene);

    virtual void pre_update(float dt) override;
    virtual void update(float dt) override;
    virtual void post_update(float dt) override;
    virtual void handle_event(Event& event, float dt) override;

   private:
    bool gui_button_clicked_event_handler(GuiSurfacePopupModalCreateButtonClickedEvent& event);
    bool gui_checkbox_changed_event_handler(GuiCheckboxChangedEvent& event);
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_C0BEZIERSURFACELAYER_HH
