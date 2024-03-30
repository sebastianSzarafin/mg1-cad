#ifndef ESPERT_SANDBOX_MATH_HH
#define ESPERT_SANDBOX_MATH_HH

#include "Espert.hh"

namespace mg1
{
  struct Plane
  {
    glm::vec3 n;
    float D;
  };

  static glm::vec3 cast_ray(float x, float y, glm::mat4 view, glm::mat4 projection)
  {
    glm::vec4 ray_clip  = glm::vec4(x, y, 1.f, 1.f);
    glm::vec4 ray_eye   = glm::inverse(projection) * ray_clip;
    glm::vec4 ray_world = glm::inverse(view) * ray_eye;
    ray_world           = normalize(ray_world);

    return { ray_world.x, ray_world.y, ray_world.z };
  }

  static glm::vec3 intersect_vector_plane(glm::vec3 start, glm::vec3 dir, Plane p)
  {
    float t = -(glm::dot(start, p.n) + p.D) / glm::dot(dir, p.n);
    
    return start + t * dir;
  }

  struct Sphere
  {
    glm::vec3 c;
    float r;
  };

  static bool intersect_vector_sphere(glm::vec3 start, glm::vec3 dir, Sphere s)
  {
    float b = glm::dot(dir, start - s.c);
    float c = glm::dot(start - s.c, start - s.c) - s.r * s.r;

    return b * b - c >= 0;
  }
} // namespace mg1

#endif // ESPERT_SANDBOX_MATH_HH
