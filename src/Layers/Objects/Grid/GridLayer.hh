#ifndef ESPERT_SANDBOX_GRIDLAYER_HH
#define ESPERT_SANDBOX_GRIDLAYER_HH

#include "Espert.hh"
#include "Events/Gui/GuiEvents.hh"
#include "GridComponent.hh"

using namespace esp;

namespace mg1
{
  class GridLayer : public Layer
  {
   private:
    Scene* m_scene;

   public:
    GridLayer(Scene* scene);

    virtual void pre_update(float dt) override;
    virtual void update(float dt) override;
    virtual void post_update(float dt) override;
    virtual void handle_event(Event& event, float dt) override;

   private:
    bool gui_checkbox_changed_event_handler(GuiCheckboxChangedEvent& event);

    void push_grid();
    void pop_grid();
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_GRIDLAYER_HH
