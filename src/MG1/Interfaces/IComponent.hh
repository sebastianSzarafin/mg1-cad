#ifndef ESPERT_SANDBOX_ICOMPONENT_HH
#define ESPERT_SANDBOX_ICOMPONENT_HH

#include "Espert.hh"

using namespace esp;

namespace mg1
{
  class IComponent
  {
   protected:
    uint32_t m_id;
    std::shared_ptr<Node> m_node;
    Scene* m_scene;

   public:
    IComponent(uint32_t id, Scene* scene = nullptr) : m_id{ id }, m_node{ Node::create() }, m_scene{ scene } {}

    inline Node* get_node() { return m_node.get(); }
    inline uint32_t get_id() { return m_id; }
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_ICOMPONENT_HH
