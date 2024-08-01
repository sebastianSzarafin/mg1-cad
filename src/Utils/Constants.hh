#ifndef ESPERT_SANDBOX_CONSTANTS_HH
#define ESPERT_SANDBOX_CONSTANTS_HH

#include "Espert.hh"

namespace mg1
{
  struct ObjectConstants
  {
    static const glm::vec3 default_color;
    static const glm::vec3 selected_color;
    static const glm::vec3 bernstein_point_color;
    static const uint32_t max_bezier_points;
  };

  inline const glm::vec3 ObjectConstants::default_color         = { 1, 1, 1 };
  inline const glm::vec3 ObjectConstants::selected_color        = { 1, 1, 0 };
  inline const glm::vec3 ObjectConstants::bernstein_point_color = { 1, .41f, .7f };
  inline const uint32_t ObjectConstants::max_bezier_points      = 100;
} // namespace mg1

#endif // ESPERT_SANDBOX_CONSTANTS_HH
