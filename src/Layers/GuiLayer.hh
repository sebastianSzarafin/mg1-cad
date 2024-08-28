#ifndef ESPERT_SANDBOX_GUILAYER_HH
#define ESPERT_SANDBOX_GUILAYER_HH

#include "Common/InitInfo.hh"
#include "Common/ObjectInfo.hh"
#include "Espert.hh"
#include "Events/Gui/GuiEvents.hh"
#include "Events/Object/ObjectEvents.hh"
#include "GuiFields/GuiFields.hh"
#include "Utils/Utils.hh"

using namespace esp;

namespace mg1
{
  class GuiLayer : public Layer
  {
   private:
    static constexpr float WIDTH  = .15f * 1920;
    static constexpr float HEIGHT = 1080;

    MouseState m_mouse_state{ GuiNotCaptured };
    CameraType m_camera_type{ Orbit };
    glm::vec3 m_mouse_cursor_pos{ 0, 0, 0 };

    std::unique_ptr<GuiCheckbox> m_grid_checkbox;
    std::unique_ptr<GuiCheckbox> m_anaglyph_mode_checkbox;
    std::unique_ptr<GuiFloatSlider> m_eye_distance_float_slider;
    std::unique_ptr<GuiFloatSlider> m_plane_distance_float_slider;
    std::unique_ptr<GuiSelectableCombo> m_actions_combo;
    std::unique_ptr<GuiObjectInfoSelectableListBox> m_objects_list_box;
    std::unique_ptr<GuiToggleButton> m_create_torus_button;
    std::unique_ptr<GuiToggleButton> m_create_point_button;
    std::unique_ptr<GuiButton> m_create_spline_button;
    std::unique_ptr<GuiButton> m_create_c2_spline_button;
    std::unique_ptr<GuiButton> m_create_c2_interpolation_spline_button;

   public:
    GuiLayer();

    void update(float dt) override;
    void handle_event(Event& event, float dt) override;

   private:
    void update_mouse_state();
    void update_camera_type(CameraType type);
    void help_marker(const char* desc);

    bool object_added_event_handler(ObjectAddedEvent& event);
    bool object_removed_event_handler(ObjectRemovedEvent& event);
    bool cursor_pos_changed_event_handler(CursorPosChangedEvent& event);
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_GUILAYER_HH
