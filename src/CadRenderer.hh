#ifndef ESPERT_SANDBOX_CADRENDERER_HH
#define ESPERT_SANDBOX_CADRENDERER_HH

#include "Espert.hh"

using namespace esp;

namespace mg1
{
  class CadRenderer
  {
   public:
    static void create();
    static void terminate();
    static void begin_scene(CameraController* camera = nullptr, Scene* scene = nullptr);
    static void end_scene();
    static void render();

    static CameraController* get_camera();
    static Scene* get_scene();

    static void set_camera(CameraController* camera);
    static void set_scene(Scene* scene);
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_CADRENDERER_HH
