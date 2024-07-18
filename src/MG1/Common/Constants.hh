#ifndef ESPERT_SANDBOX_CONSTANTS_HH
#define ESPERT_SANDBOX_CONSTANTS_HH

#include "Espert.hh"

namespace mg1
{
  inline static const glm::mat4 BERNSTEIN_BASE{ 1, 0, 0, 0, -3, 3, 0, 0, 3, -6, 3, 0, -1, 3, -3, 1 };
  inline static const glm::mat4 BSPLINE_BASE = glm::mat4{ 1, 4, 1, 0, -3, 0, 3, 0, 3, -6, 3, 0, -1, 3, -3, 1 } / 6.f;
} // namespace mg1
#endif // ESPERT_SANDBOX_CONSTANTS_HH
