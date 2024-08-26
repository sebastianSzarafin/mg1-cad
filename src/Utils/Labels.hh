#ifndef ESPERT_SANDBOX_LABELS_HH
#define ESPERT_SANDBOX_LABELS_HH

#include "Espert.hh"

namespace mg1
{
  struct GuiLabel
  {
    static const std::string rotation_axis;
    static const std::string objects;
    static const std::string object_none;
    static const std::string actions;
    static const std::string action_none;
    static const std::string action_rot_ox;
    static const std::string action_rot_oy;
    static const std::string action_rot_oz;
    static const std::string action_scale;
    static const std::string action_scale_ox;
    static const std::string action_scale_oy;
    static const std::string action_scale_oz;
    static const std::string create_torus_button;
    static const std::string create_point_button;
    static const std::string create_spline_button;
    static const std::string create_c2_spline_button;
    static const std::string create_c2_interpolation_spline_button;
    static const std::string remove_object_button;
    static const std::string rename_object_button;
    static const std::string grid_checkbox;
    static const std::string action_set_cursor_pos;
    static const std::string gui_camera_type_changed_event;
    static const std::string control_line_checkbox;
    static const std::string m_spline_base_radio_buttons;
  };

  inline const std::string GuiLabel::rotation_axis                         = "Rotation axis";
  inline const std::string GuiLabel::objects                               = "Objects";
  inline const std::string GuiLabel::object_none                           = "No object";
  inline const std::string GuiLabel::actions                               = "Actions";
  inline const std::string GuiLabel::action_none                           = "No action";
  inline const std::string GuiLabel::action_rot_ox                         = "Rotate around OX";
  inline const std::string GuiLabel::action_rot_oy                         = "Rotate around OY";
  inline const std::string GuiLabel::action_rot_oz                         = "Rotate around OZ";
  inline const std::string GuiLabel::action_scale                          = "Scale";
  inline const std::string GuiLabel::action_scale_ox                       = "Scale OX";
  inline const std::string GuiLabel::action_scale_oy                       = "Scale OY";
  inline const std::string GuiLabel::action_scale_oz                       = "Scale OZ";
  inline const std::string GuiLabel::create_torus_button                   = "Create torus";
  inline const std::string GuiLabel::create_point_button                   = "Create point";
  inline const std::string GuiLabel::create_spline_button                  = "Create spline";
  inline const std::string GuiLabel::create_c2_spline_button               = "Create C2 spline";
  inline const std::string GuiLabel::create_c2_interpolation_spline_button = "Create C2 interpolation spline";
  inline const std::string GuiLabel::remove_object_button                  = "Remove";
  inline const std::string GuiLabel::rename_object_button                  = "Rename";
  inline const std::string GuiLabel::grid_checkbox                         = "Grid";
  inline const std::string GuiLabel::action_set_cursor_pos                 = "Set cursor position";
  inline const std::string GuiLabel::gui_camera_type_changed_event         = "Camera type changed event";
  inline const std::string GuiLabel::control_line_checkbox                 = "Control line";
  inline const std::string GuiLabel::m_spline_base_radio_buttons           = "Spline base";

  struct ObjectLabel
  {
    static const std::string object_created_event;
    static const std::string object_removed_event;
    static const std::string cursor_pos_changed_event;
    static const std::string mouse_cursor;
    static const std::string object_cursor;
    static const std::string object_mass_centre_changed_event;
  };

  inline const std::string ObjectLabel::object_created_event             = "Object created event";
  inline const std::string ObjectLabel::object_removed_event             = "Object removed event";
  inline const std::string ObjectLabel::cursor_pos_changed_event         = "Cursor pos changed event";
  inline const std::string ObjectLabel::mouse_cursor                     = "Mouse cursor";
  inline const std::string ObjectLabel::object_cursor                    = "Object cursor";
  inline const std::string ObjectLabel::object_mass_centre_changed_event = "Object mass centre changed event";
} // namespace mg1

#endif // ESPERT_SANDBOX_LABELS_HH
