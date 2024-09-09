#ifndef ESPERT_SANDBOX_CURSORROTCHANGEDEVENT_HH
#define ESPERT_SANDBOX_CURSORROTCHANGEDEVENT_HH

#include "ObjectEvent.hh"
#include "Utils/Enums.hh"
#include "Utils/Labels.hh"

using namespace esp;

namespace mg1
{
  class CursorRotChangedEvent : public ObjectEvent
  {
   private:
    CursorType m_type;
    glm::quat m_scale;

   public:
    CursorRotChangedEvent(CursorType type, glm::quat rotation) :
        ObjectEvent(ObjectLabel::cursor_rot_changed_event), m_type{ type }, m_scale{ rotation }
    {
    }

    inline bool is_type(CursorType type) { return m_type == type; }
    inline glm::quat get_rotation() { return m_scale; }

    EVENT_CLASS_TYPE(EventTypeLayer)
    EVENT_CLASS_SUBTYPE(Nothing)
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_CURSORROTCHANGEDEVENT_HH
