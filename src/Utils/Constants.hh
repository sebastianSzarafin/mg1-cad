#ifndef ESPERT_SANDBOX_CONSTANTS_HH
#define ESPERT_SANDBOX_CONSTANTS_HH

#include "Espert.hh"

namespace mg1
{
  struct CursorInit
  {
    static const esp::ModelParams S_MODEL_PARAMS;
  };

  inline const esp::ModelParams CursorInit::S_MODEL_PARAMS = { .m_position = true, .m_color = true };

  struct GridInit
  {
    static const esp::ModelParams S_MODEL_PARAMS;
  };

  inline const esp::ModelParams GridInit::S_MODEL_PARAMS = { .m_position = true, .m_color = true };

  struct TorusInit
  {
    static constexpr float S_R           = 1.f;
    static constexpr float S_r           = .3f;
    static constexpr int S_DENSITY_THETA = 30; // how many circles
    static constexpr int S_DENSITY_PHI   = 20; // how many points on a circle
    static const esp::ModelParams S_MODEL_PARAMS;
  };

  inline const esp::ModelParams TorusInit::S_MODEL_PARAMS = { .m_position = true };

  struct PointInit
  {
    static constexpr float S_R = .025f;
    static const esp::ModelParams S_MODEL_PARAMS;
  };

  inline const esp::ModelParams PointInit::S_MODEL_PARAMS = { .m_position = true };

  struct SplineInit
  {
    static const esp::ModelParams S_MODEL_PARAMS;
  };

  inline const esp::ModelParams SplineInit::S_MODEL_PARAMS = { .m_position = true };

  struct SurfaceInit
  {
    static const esp::ModelParams S_MODEL_PARAMS;
  };

  inline const esp::ModelParams SurfaceInit::S_MODEL_PARAMS = { .m_position = true };

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

  struct WindowConstants
  {
    static const float root_win_w;
    static const float root_win_h;
    static const float root_win_avail_w;
    static const ImVec2 gui_win_size;
    static const float gui_scene_splitter_size;
    static const ImVec2 scene_win_size;
    static const ImVec2 scene_win_pos; // (left, top)
  };

  inline const float WindowConstants::root_win_w              = 1920.f;
  inline const float WindowConstants::root_win_h              = 1080.f;
  inline const float WindowConstants::root_win_avail_w        = 1904.f;
  inline const ImVec2 WindowConstants::gui_win_size           = { .2f * WindowConstants::root_win_avail_w, 0 };
  inline const float WindowConstants::gui_scene_splitter_size = .01f * WindowConstants::root_win_avail_w;
  inline const ImVec2 WindowConstants::scene_win_size         = { .79f * WindowConstants::root_win_avail_w,
                                                                  .79f * WindowConstants::root_win_h };
  inline const ImVec2 WindowConstants::scene_win_pos          = { 407, 27 };
} // namespace mg1

#endif // ESPERT_SANDBOX_CONSTANTS_HH
