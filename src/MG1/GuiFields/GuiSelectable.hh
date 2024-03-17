#ifndef ESPERT_SANDBOX_GUISELECTABLE_HH
#define ESPERT_SANDBOX_GUISELECTABLE_HH

#include "GuiField.hh"
#include "MG1/Common/ObjectInfo.hh"

namespace mg1
{
  class GuiSelectable;
  using GuiSelectables = std::vector<std::shared_ptr<GuiSelectable>>;

  class GuiSelectable : public GuiField<bool>
  {
   protected:
    bool m_prev_selected;
    bool m_changed;

   public:
    GuiSelectable(const std::string& label, bool value) : GuiField<bool>(label, value), m_prev_selected{ value } {}

    inline void render()
    {
      if ((m_changed = ImGui::Selectable(m_label.c_str(), m_value)))
      {
        if (!m_prev_selected) { select(); }
        else { unselect(); }
      }

      m_prev_selected = m_value;
    }

    inline bool is_selected() { return m_value; }
    inline bool changed() override { return m_changed; }
    virtual inline void select()
    {
      m_value = true;
      create_and_post_event();
    }
    virtual inline void unselect()
    {
      m_value = false;
      create_and_post_event();
    }

    virtual inline void create_and_post_event() override
    {
      GuiSelectableChangedEvent event{ m_label, m_value };
      post_event(event);
    }
  };

  class GuiObjectInfoSelectable : public GuiSelectable
  {
   private:
    ObjectInfo* m_info;

   public:
    GuiObjectInfoSelectable(ObjectInfo* info) : GuiSelectable(info->m_name, info->m_is_selected), m_info{ info } {}

    inline void render() override
    {
      if ((m_changed = ImGui::Selectable(m_label.c_str(), m_value)))
      {
        if (!m_prev_selected) { select(); }
        else { unselect(); }
      }

      if (m_value) { m_info->render(); }

      m_prev_selected = m_value;
    }

    inline void select() override
    {
      m_value = true;
      m_info->select();
      create_and_post_event();
    }
    inline void unselect() override
    {
      m_value = false;
      m_info->unselect();
      create_and_post_event();
    }
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_GUISELECTABLE_HH
