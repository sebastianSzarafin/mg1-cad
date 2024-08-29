#ifndef ESPERT_SANDBOX_TORUSLAYER_HH
#define ESPERT_SANDBOX_TORUSLAYER_HH

#include "Espert.hh"
#include "Events/Gui/GuiEvents.hh"

using namespace esp;

namespace mg1
{
  class TorusLayer : public Layer
  {
   private:
    Scene* m_scene;

    bool m_create_torus_toggle_on{ false };

   public:
    TorusLayer(Scene* scene);

    virtual void pre_update(float dt) override;
    virtual void update(float dt) override;
    virtual void post_update(float dt) override;
    virtual void handle_event(Event& event, float dt) override;

   private:
    bool gui_toggle_button_clicked_event_handler(GuiToggleButtonClickedEvent& event);
    bool mouse_button_pressed_event_handler(MouseButtonPressedEvent& event);

    glm::vec3 get_cursor_pos();
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_TORUSLAYER_HH
