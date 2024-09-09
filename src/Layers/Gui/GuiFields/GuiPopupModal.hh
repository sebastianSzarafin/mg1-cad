#ifndef ESPERT_SANDBOX_GUIPOPUPMODAL_HH
#define ESPERT_SANDBOX_GUIPOPUPMODAL_HH

#include "GuiField.hh"
#include "Utils/Enums.hh"

namespace mg1
{
  class GuiPopupModal : public GuiField<void*>
  {
   protected:
    ImGuiWindowFlags m_flags;

   public:
    GuiPopupModal(const std::string& label, ImGuiWindowFlags flags = 0) : GuiField(label, nullptr), m_flags{ flags } {}

    virtual void open() { ImGui::OpenPopup(m_label.c_str()); }

    inline void render() override
    {
      if (ImGui::BeginPopupModal(m_label.c_str(), nullptr, m_flags))
      {
        render_body();

        ImGui::EndPopup();
      }
    }

   protected:
    virtual void render_body() = 0;
  };

  class GuiSurfacePopupModal : public GuiPopupModal
  {
   private:
    int m_surface_type;
    SurfaceContinuity m_continuity;

    std::shared_ptr<GuiDragFloat> m_radius_drag;
    std::shared_ptr<GuiDragFloat> m_width_drag;
    std::shared_ptr<GuiDragFloat> m_height_drag;
    std::shared_ptr<GuiInputInt> m_segments_u_input_int;
    std::shared_ptr<GuiInputInt> m_segments_v_input_int;

   public:
    GuiSurfacePopupModal(ImGuiWindowFlags flags = 0) :
        GuiPopupModal(GuiLabel::m_create_surface_popup_modal, flags), m_surface_type{ SurfaceType::Flat }
    {
      m_radius_drag          = std::make_shared<GuiDragFloat>("Radius", 1.5f, .1f, .1f, FLT_MAX);
      m_width_drag           = std::make_shared<GuiDragFloat>("Width", 5.f, .1f, .1f, FLT_MAX);
      m_height_drag          = std::make_shared<GuiDragFloat>("Height", 5.f, .1f, .1f, FLT_MAX);
      m_segments_u_input_int = std::make_shared<GuiInputInt>("Segments (u)", 1);
      m_segments_v_input_int = std::make_shared<GuiInputInt>("Segments (v)", 1);
    }

    inline void set_continuity(SurfaceContinuity continuity) { m_continuity = continuity; }

    inline void open() override
    {
      ImVec2 center = ImGui::GetMainViewport()->GetCenter();
      ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

      GuiPopupModal::open();
    }

    inline void render_body() override
    {
      ImGui::RadioButton("Flat", &m_surface_type, (int)SurfaceType::Flat);
      ImGui::SameLine();
      ImGui::RadioButton("Cylinder", &m_surface_type, (int)SurfaceType::Cylinder);
      ImGui::Spacing();
      if (m_surface_type == SurfaceType::Flat) { m_width_drag->render(); }
      else { m_radius_drag->render(); }
      m_height_drag->render();
      ImGui::Spacing();
      if (m_surface_type == SurfaceType::Flat) { m_segments_u_input_int->set_min_value(1); }
      else { m_segments_u_input_int->set_min_value(2); }
      m_segments_u_input_int->render();
      m_segments_v_input_int->render();
      ImGui::Spacing();

      if (ImGui::Button("Create", ImVec2(120, 0)))
      {
        create_and_post_event();
        ImGui::CloseCurrentPopup();
      }
      ImGui::SetItemDefaultFocus();
      ImGui::SameLine();
      if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
    }

    inline void create_and_post_event() override
    {
      auto width = m_surface_type == SurfaceType::Flat ? m_width_drag->get_value() : m_radius_drag->get_value();
      GuiSurfacePopupModalCreateButtonClickedEvent event{ GuiLabel::m_surface_popup_modal_create_button,
                                                          { (SurfaceType)m_surface_type,
                                                            m_continuity,
                                                            width,
                                                            m_height_drag->get_value(),
                                                            m_segments_u_input_int->get_value(),
                                                            m_segments_v_input_int->get_value() } };
      post_event(event);
    }
  };

} // namespace mg1

#endif // ESPERT_SANDBOX_GUIPOPUPMODAL_HH
