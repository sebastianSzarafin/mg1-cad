#ifndef ESPERT_SANDBOX_OBJECTREMOVEDEVENT_HH
#define ESPERT_SANDBOX_OBJECTREMOVEDEVENT_HH

#include "MG1/Common/ObjectInfo.hh"
#include "MG1/Utils/Utils.hh"
#include "ObjectEvent.hh"

using namespace esp;

namespace mg1
{
  class ObjectRemovedEvent : public ObjectEvent
  {
   private:
    ObjectInfo* m_info;

   public:
    ObjectRemovedEvent(ObjectInfo* info) : ObjectEvent(ObjectLabel::object_removed_event), m_info{ info } {}

    inline ObjectInfo* get_info() { return m_info; }
    inline std::string get_name() { return m_info->m_name; }

    EVENT_CLASS_TYPE(EventTypeLayer)
    EVENT_CLASS_SUBTYPE(Nothing)
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_OBJECTREMOVEDEVENT_HH
