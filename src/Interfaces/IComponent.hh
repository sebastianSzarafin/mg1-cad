#ifndef ESPERT_SANDBOX_ICOMPONENT_HH
#define ESPERT_SANDBOX_ICOMPONENT_HH

#include "Espert.hh"

using namespace esp;

namespace mg1
{
  class IComponent
  {
   protected:
    entt::entity m_id;

   public:
    IComponent(entt::entity id) : m_id{ id } {}

    inline uint32_t get_id() { return (uint32_t)m_id; }
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_ICOMPONENT_HH
