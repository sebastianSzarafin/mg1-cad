#ifndef ESPERT_SANDBOX_OBJECTINFO_HH
#define ESPERT_SANDBOX_OBJECTINFO_HH

#include "Espert.hh"
#include "MG1/GuiFields/GuiCheckbox.hh"
#include "MG1/GuiFields/GuiRadioButtons.hh"
#include "MG1/Utils/Utils.hh"

namespace mg1
{
  struct ObjectInfo
  {
   public:
    enum class ObjectState
    {
      Default  = 0,
      Selected = 1,
      Removed  = 2
    };

   protected:
    ObjectState m_state;
    bool m_renameable{ false };
    bool m_removeable{ false };
    bool m_visible{ true };

   public:
    uint32_t m_id;
    std::string m_name;

    ObjectInfo(uint32_t id, const std::string& name) : m_id{ id }, m_name{ name }, m_state{ ObjectState::Default } {}

    virtual inline void select()
    {
      if (m_state == ObjectState::Default) { m_state = ObjectState::Selected; }
    }
    virtual inline void unselect()
    {
      if (m_state == ObjectState::Selected) { m_state = ObjectState::Default; }
    }
    virtual inline void remove() { m_state = ObjectState::Removed; }
    virtual inline void set_visibility(bool visibility) { m_visible = visibility; }

    inline bool selected() { return m_state == ObjectState::Selected; }
    inline bool removed() { return m_state == ObjectState::Removed; }
    inline bool is_renameable() { return m_renameable; }
    inline bool is_removeable() { return m_removeable; }
    inline bool is_visible() { return m_visible; }

    virtual void render() = 0;
  };

  struct TorusInfo : public ObjectInfo
  {
    float m_R;
    float m_r;
    int m_density_theta;
    int m_density_phi;

    bool m_dirty{ false };

    TorusInfo(uint32_t id, const std::string& name) : ObjectInfo(id, name) { m_renameable = m_removeable = true; }

    inline void render() override
    {
      ImGui::SeparatorText("Info:");
      ImGui::Spacing();
      ImGui::InputText("Name", &m_name);
      ImGui::InputFloat("R", &m_R, 0.05f, 1.0f, "%.2f");
      if (ImGui::IsItemDeactivatedAfterEdit()) { m_dirty = true; }
      ImGui::InputFloat("r", &m_r, 0.05f, 1.0f, "%.2f");
      if (ImGui::IsItemDeactivatedAfterEdit()) { m_dirty = true; }
      ImGui::InputInt("Density - theta", &m_density_theta, 1, 100);
      if (ImGui::IsItemDeactivatedAfterEdit()) { m_dirty = true; }
      ImGui::InputInt("Density - phi", &m_density_phi, 1, 100);
      if (ImGui::IsItemDeactivatedAfterEdit()) { m_dirty = true; }
      ImGui::Spacing();
    };
  };

  struct PointInfo : public ObjectInfo
  {
   private:
    static int s_selected_index;

   public:
    float m_r;

    bool m_dirty{ false };

    int m_selected_index{ -1 };

    PointInfo(uint32_t id, const std::string& name) : ObjectInfo(id, name) { m_renameable = m_removeable = true; }

    inline void select() override
    {
      if (m_state == ObjectState::Default) { m_state = ObjectState::Selected; }
      m_selected_index = ++PointInfo::s_selected_index;
    }
    inline void unselect() override
    {
      if (m_state == ObjectState::Selected) { m_state = ObjectState::Default; }
      PointInfo::s_selected_index--;
      m_selected_index = -1;
    }

    inline void render() override
    {
      ImGui::SeparatorText("Info:");
      ImGui::Spacing();
      ImGui::InputText("Name", &m_name);
      ImGui::InputFloat("R", &m_r, 0.05f, 1.0f, "%.2f");
      if (ImGui::IsItemDeactivatedAfterEdit()) { m_dirty = true; }
      ImGui::Spacing();
    }
  };

  inline int PointInfo::s_selected_index = -1;

  struct SplineInfo : public ObjectInfo
  {
   private:
    std::shared_ptr<GuiCheckbox> m_control_line_checkbox;

   public:
    std::vector<PointInfo*> m_control_points;

    bool m_dirty{ false };

    SplineInfo(uint32_t id, const std::string& name, std::vector<PointInfo*> control_points) :
        ObjectInfo(id, name), m_control_points{ control_points }
    {
      m_control_line_checkbox = std::make_shared<GuiCheckbox>(GuiLabel::control_line_checkbox, false);

      m_removeable = true;
    }

    inline void select() override
    {
      if (m_state == ObjectState::Default) { m_state = ObjectState::Selected; }
      for (auto point : m_control_points)
      {
        point->select();
      }
    }
    inline void unselect() override
    {
      if (m_state == ObjectState::Selected) { m_state = ObjectState::Default; }
      for (auto point : m_control_points)
      {
        point->unselect();
      }
    }

    inline void render() override
    {
      ImGui::SeparatorText("Info:");
      ImGui::Spacing();
      for (auto point : m_control_points)
      {
        ImGui::Text("%s", point->m_name.c_str());
      }
      ImGui::Spacing();
      m_control_line_checkbox->render();
      ImGui::Spacing();
    }
  };

  struct C2SplineInfo : public SplineInfo
  {
   private:
    std::shared_ptr<GuiRadioButtons> m_spline_base_radio_buttons;

   public:
    C2SplineInfo(uint32_t id, const std::string& name, std::vector<PointInfo*> control_points) :
        SplineInfo(id, name, control_points)
    {
      m_spline_base_radio_buttons =
          std::make_shared<GuiRadioButtons>(GuiLabel::m_spline_base_radio_buttons,
                                            0,
                                            std::vector<std::string>{ "Bernstein", "B-Spline" });
    }

    inline void render() override
    {
      SplineInfo::render();

      ImGui::Text("Spline base: ");
      m_spline_base_radio_buttons->render();
    }
  };

  struct CursorInfo : public ObjectInfo
  {
    CursorType m_type;
    glm::vec3 m_position;

   public:
    CursorInfo(uint32_t id, const std::string& name, CursorType type, glm::vec3 position) :
        ObjectInfo(id, name), m_type{ type }, m_position{ position }
    {
    }

    inline void render() override
    {
      ImGui::SeparatorText("Info:");
      ImGui::Text("Name %s", m_name.c_str());
      ImGui::Spacing();
      if (m_type == CursorType::Mouse)
      {
        ImGui::InputFloat("x", &m_position.x, 0.05f, 1.0f, "%.2f");
        ImGui::InputFloat("z", &m_position.z, 0.05f, 1.0f, "%.2f");
      }
      ImGui::Spacing();
    }
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_OBJECTINFO_HH
