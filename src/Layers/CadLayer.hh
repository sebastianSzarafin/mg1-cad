#ifndef ESPERT_SANDBOX_CADLAYER_HH
#define ESPERT_SANDBOX_CADLAYER_HH

#include "Espert.hh"
#include "Events/Gui/GuiEvents.hh"
#include "Utils/Math.hh"

using namespace esp;

namespace mg1
{
  class CadLayer : public Layer
  {
   private:
    struct
    {
      std::shared_ptr<EspBlock> m_block;
      std::shared_ptr<EspDepthBlock> m_depth_block;
      std::unique_ptr<EspRenderPlan> m_plan;
    } m_scene_render;
    struct
    {
      std::shared_ptr<EspBlock> m_block;
      std::shared_ptr<EspDepthBlock> m_depth_block;
      std::unique_ptr<EspRenderPlan> m_plan;
    } m_anaglyph_mode_render;
    struct
    {
      std::unique_ptr<EspRenderPlan> m_plan;
      std::shared_ptr<EspShader> m_shader;
      std::unique_ptr<EspUniformManager> m_uniform_manager;
      struct
      {
        std::shared_ptr<EspShader> m_shader;
        std::unique_ptr<EspUniformManager> m_uniform_manager;
      } m_anaglyph_mode;
      std::unique_ptr<EspVertexBuffer> m_vertex_buffer;
      std::unique_ptr<EspIndexBuffer> m_index_buffer;
    } m_final_render;

    std::shared_ptr<OrbitCameraController> m_orbit_camera;
    std::shared_ptr<FpsCameraController> m_fps_camera;
    std::shared_ptr<Scene> m_scene;

    std::unique_ptr<Layer> m_gui_layer;
    std::unique_ptr<Layer> m_object_layer;

    bool m_none_object_selected{ true };
    bool m_mouse_captured{ true };
    bool m_none_action_selected{ true };

    struct
    {
      bool m_on{ false };
      float m_eye_dist{ .01f };
      float m_plane_dist{ 1 };
    } m_anaglyph_mode;

   public:
    CadLayer();

    void pre_update(float dt) override;
    void update(float dt) override;
    void post_update(float dt) override;
    void handle_event(Event& event, float dt) override;

   private:
    bool mouse_moved_event_handler(MouseMovedEvent& event, float dt);
    bool gui_selectable_changed_event_handler(GuiSelectableChangedEvent& event);
    bool gui_mouse_state_changed_event_handler(GuiMouseStateChangedEvent& event);
    bool gui_camera_type_changed_event_handler(GuiCameraTypeChangedEvent& event);
    bool gui_checkbox_changed_event_handler(GuiCheckboxChangedEvent& event);
    bool gui_float_slider_changed_event_handler(GuiFloatSliderChangedEvent& event);

    void update_camera_on_scene(glm::mat4 projection, glm::mat4 view);

    inline bool fps_camera_selected() { return Scene::get_current_camera() == m_fps_camera.get(); }
    inline bool orbit_camera_selected() { return Scene::get_current_camera() == m_orbit_camera.get(); }
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_CADLAYER_HH
