#ifndef ESPERT_SANDBOX_GRIDCOMPONENT_HH
#define ESPERT_SANDBOX_GRIDCOMPONENT_HH

#include "Espert.hh"
#include "Interfaces/Interfaces.hh"

using namespace esp;

namespace mg1
{
  struct GridComponentParams
  {
    int m_threshold;
    float m_scale;
    glm::vec3 m_color;
    float m_y;
  };

  class GridComponent : public IComponent
  {
   public:
    GridComponent(entt::entity id);

    static std::tuple<std::vector<Vertex>, std::vector<uint32_t>> construct(GridComponentParams params);
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_GRIDCOMPONENT_HH
