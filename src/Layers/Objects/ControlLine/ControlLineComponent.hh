#ifndef ESPERT_SANDBOX_CONTROLLINECOMPONENT_HH
#define ESPERT_SANDBOX_CONTROLLINECOMPONENT_HH

#include "Espert.hh"
#include "Interfaces/Interfaces.hh"

using namespace esp;

namespace mg1
{
  class ControlLineComponent : public IComponent
  {
   private:
    uint32_t m_vertex_count;

   public:
    ControlLineComponent(uint32_t id, uint32_t vertex_count);
    ~ControlLineComponent() = default;

    inline void set_vertex_count(uint32_t val) { m_vertex_count = val; }

    std::vector<uint32_t> get_indices();
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_CONTROLLINECOMPONENT_HH
