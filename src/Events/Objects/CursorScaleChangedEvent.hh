#ifndef ESPERT_SANDBOX_CURSORSCALECHANGEDEVENT_HH
#define ESPERT_SANDBOX_CURSORSCALECHANGEDEVENT_HH

#include "ObjectEvent.hh"
#include "Utils/Enums.hh"
#include "Utils/Labels.hh"

using namespace esp;

namespace mg1
{
  class CursorScaleChangedEvent : public ObjectEvent
  {
   private:
    CursorType m_type;
    glm::vec3 m_scale;

   public:
    CursorScaleChangedEvent(CursorType type, glm::vec3 scale) :
        ObjectEvent(ObjectLabel::cursor_scale_changed_event), m_type{ type }, m_scale{ scale }
    {
    }

    inline bool is_type(CursorType type) { return m_type == type; }
    inline glm::vec3 get_scale() { return m_scale; }

    EVENT_CLASS_TYPE(EventTypeLayer)
    EVENT_CLASS_SUBTYPE(Nothing)
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_CURSORSCALECHANGEDEVENT_HH
