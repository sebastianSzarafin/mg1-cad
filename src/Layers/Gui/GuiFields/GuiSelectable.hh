#ifndef ESPERT_SANDBOX_GUISELECTABLE_HH
#define ESPERT_SANDBOX_GUISELECTABLE_HH

#include "GuiButton.hh"
#include "GuiField.hh"
#include "Layers/Objects/ObjectInfo.hh"

namespace mg1
{
  template<typename T> class GuiSelectable;
  template<typename T> using GuiSelectables = std::vector<std::shared_ptr<GuiSelectable<T>>>;

  template<typename T> class GuiSelectable : public GuiField<T>
  {
   protected:
    bool m_selected;
    bool m_prev_selected;
    bool m_changed;

   public:
    GuiSelectable(const std::string& label, T value, bool selected) :
        GuiField<T>(label, value), m_selected{ selected }, m_prev_selected{ selected }
    {
    }

    inline void render() override
    {
      if ((m_changed = ImGui::Selectable(this->m_label.c_str(), m_selected)))
      {
        if (!m_prev_selected) { select(); }
        else { unselect(); }
      }

      m_prev_selected = m_selected;
    }

    inline bool is_selected() { return m_selected; }
    inline bool changed() override { return m_changed; }
    virtual inline void select()
    {
      m_selected = true;
      create_and_post_event();
    }
    virtual inline void unselect()
    {
      m_selected = false;
      create_and_post_event();
    }

    virtual inline void create_and_post_event() override
    {
      GuiSelectableChangedEvent event{ this->m_label, m_selected };
      this->post_event(event);
    }
  };

  class GuiObjectInfoSelectable : public GuiSelectable<ObjectInfo*>
  {
   private:
    std::shared_ptr<GuiButton> m_rename_button;
    std::shared_ptr<GuiButton> m_remove_button;

   public:
    GuiObjectInfoSelectable(ObjectInfo* info) : GuiSelectable(info->m_name, info, info->selected())
    {
      m_rename_button = std::make_shared<GuiButton>(GuiLabel::rename_object_button);
      m_rename_button->set_max_width();
      m_remove_button = std::make_shared<GuiButton>(GuiLabel::remove_object_button);
      m_remove_button->set_max_width();
    }

    inline void render() override
    {
      m_selected = this->m_value->selected();

      if (!this->m_value->is_visible())
      {
        m_changed = false;
        return;
      }

      if ((m_changed = ImGui::Selectable(m_label.c_str(), m_selected)))
      {
        if (!m_prev_selected) { select(); }
        else { unselect(); }
      }

      if (ImGui::BeginPopupContextItem())
      {
        m_changed = true;
        select();
        this->m_value->render();
        if (this->m_value->is_renameable())
        {
          m_rename_button->render();
          if (m_rename_button->clicked() && !this->m_value->m_name.empty()) { m_label = this->m_value->m_name; }
        }
        if (this->m_value->is_removeable())
        {
          m_remove_button->render();
          if (m_remove_button->clicked()) { this->m_value->remove(); }
        }

        ImGui::EndPopup();
      }

      m_prev_selected = m_selected;
    }

    inline void select() override
    {
      m_selected = true;
      this->m_value->select();
      create_and_post_event();
    }
    inline void unselect() override
    {
      m_selected = false;
      this->m_value->unselect();
      create_and_post_event();
    }
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_GUISELECTABLE_HH
