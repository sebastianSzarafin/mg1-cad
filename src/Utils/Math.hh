#ifndef ESPERT_SANDBOX_MATH_HH
#define ESPERT_SANDBOX_MATH_HH

#include "Constants.hh"
#include "Espert.hh"

#define CLIP_SCALE(a, b) ((a) / b * 2.f - 1.f) // scale 'a' from [0,b] to [-1,1]

namespace mg1
{
  inline static const float mouse_offset_x =
      (WindowConstants::scene_win_pos.x + WindowConstants::scene_win_size.x / 2 - WindowConstants::root_win_w / 2) * 2;
  inline static const float mouse_offset_y =
      (WindowConstants::scene_win_pos.y + WindowConstants::scene_win_size.y / 2 - WindowConstants::root_win_h / 2) / 4;

  class Math
  {
   public:
    struct Plane
    {
      glm::vec3 n;
      float D;
    };

    struct Sphere
    {
      glm::vec3 c;
      float r;
    };

    static glm::vec3 s_mouse_ray;

    static glm::vec3 cast_ray(float x, float y, glm::mat4 view, glm::mat4 projection)
    {
      glm::vec4 ray_clip  = glm::vec4(x, y, 1.f, 1.f);
      glm::vec4 ray_eye   = glm::inverse(projection) * ray_clip;
      glm::vec4 ray_world = glm::inverse(view) * ray_eye;
      ray_world           = glm::normalize(ray_world);

      return { ray_world.x, ray_world.y, ray_world.z };
    }

    static glm::vec3 intersect_vector_plane(glm::vec3 start, glm::vec3 dir, Plane p)
    {
      float t = -(glm::dot(start, p.n) + p.D) / glm::dot(dir, p.n);

      return start + t * dir;
    }

    static bool intersect_vector_sphere(glm::vec3 start, glm::vec3 dir, Sphere s)
    {
      float b = glm::dot(dir, start - s.c);
      float c = glm::dot(start - s.c, start - s.c) - s.r * s.r;

      return b * b - c >= 0;
    }

    static float get_mouse_x_cs()
    {
      float window_width, mouse_x = EspInput::get_mouse_x();
      if (EspGui::m_use_gui)
      {
        window_width = WindowConstants::scene_win_size.x;
        mouse_x -= mouse_offset_x;
      }
      else { window_width = EspWindow::get_width(); }

      return mouse_x / window_width * 2.f - 1.f;
    }

    static float get_mouse_y_cs()
    {
      float window_height, mouse_y = EspInput::get_mouse_y();
      if (EspGui::m_use_gui)
      {
        window_height = WindowConstants::scene_win_size.y;
        mouse_y += mouse_offset_y;
      }
      else { window_height = EspWindow::get_height(); }

      return 1.f - mouse_y / window_height * 2.f;
    }

    static void on_new_frame()
    {
      auto camera = Scene::get_current_camera();

      s_mouse_ray =
          Math::cast_ray(Math::get_mouse_x_cs(), Math::get_mouse_y_cs(), camera->get_view(), camera->get_projection());
    }
  };

  inline glm::vec3 Math::s_mouse_ray = glm::vec3();

} // namespace mg1

#endif // ESPERT_SANDBOX_MATH_HH
