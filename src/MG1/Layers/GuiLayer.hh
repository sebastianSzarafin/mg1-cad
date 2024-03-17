#ifndef ESPERT_SANDBOX_GUILAYER_HH
#define ESPERT_SANDBOX_GUILAYER_HH

#include "Espert.hh"
#include "MG1/Events/Gui/GuiEvents.hh"
#include "MG1/Events/Object/ObjectEvents.hh"
#include "MG1/GuiFields/GuiFields.hh"
#include "MG1/Utils.hh"

using namespace esp;

namespace mg1
{
  class GuiLayer : public Layer
  {
   private:
    static constexpr ImVec2 OFFSET{ 10, 10 };

    MouseState m_mouse_state{ GuiNotCaptured };

    std::unique_ptr<GuiObjectInfoSelectableListBox> m_objects_list_box;

   public:
    GuiLayer();

    void update(float dt) override;
    void handle_event(Event& event, float dt) override;

   private:
    void update_mouse_state();
    void help_marker(const char* desc);

    bool object_added_event_handler(ObjectAddedEvent& event);
    // bool object_removed_event_handler(ObjectRemovedEvent& event);
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_GUILAYER_HH
