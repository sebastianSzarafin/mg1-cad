#ifndef ESPERT_SANDBOX_GUISELECTABLELISTBOX_HH
#define ESPERT_SANDBOX_GUISELECTABLELISTBOX_HH

#include "Events/Gui/GuiEvents.hh"
#include "GuiField.hh"
#include "GuiSelectable.hh"
#include "Layers/Objects/Point/PointInfo.hh"

namespace mg1
{
  template<typename T> class GuiSelectableListBox : public GuiField<GuiSelectables<T>>
  {
   public:
    GuiSelectableListBox(const std::string& label, GuiSelectables<T> value = {}) :
        GuiField<GuiSelectables<T>>(label, value)
    {
    }

    inline void render() override
    {
      if (ImGui::BeginListBox(this->m_label.c_str(), ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing())))
      {
        render_body();

        ImGui::EndListBox();
      }

      create_and_post_event();
    }

    inline bool is_any_selected()
    {
      for (auto& selectable : this->m_value)
      {
        if (selectable->is_selected()) { return true; }
      }
      return false;
    }

    inline int selected_count()
    {
      int count = 0;
      for (auto& selectable : this->m_value)
      {
        if (selectable->is_selected()) { count++; }
      }
      return count;
    }

    inline void create_and_post_event() override
    {
      GuiSelectableListBoxAnySelectedEvent event{ this->m_label, is_any_selected() };
      this->post_event(event);
    }

   protected:
    virtual void render_body()
    {
      for (auto& selectable : this->m_value)
      {
        selectable->render();

        if (selectable->changed())
        {
          if (!ImGui::GetIO().KeyCtrl) { clear(); }
          selectable->select();
        }
      }
    }

    void clear()
    {
      for (auto& selectable : this->m_value)
      {
        selectable->unselect();
      }
    }
  };

  class GuiObjectInfoSelectableListBox : public GuiSelectableListBox<ObjectInfo*>
  {
   private:
    bool m_hide_points{ false };

   public:
    GuiObjectInfoSelectableListBox(const std::string& label, GuiSelectables<ObjectInfo*> value = {}) :
        GuiSelectableListBox(label, value)
    {
    }

    inline void render() override
    {
      auto selected = selected_count();
      if (selected > 1 && m_value[0]->is_selected()) { m_value[0]->unselect(); }
      else if (selected == 0) { m_value[0]->select(); }

      GuiSelectableListBox::render();
    }

    inline void handle_event(ObjectAddedEvent& event)
    {
      auto info = event.get_info();
      GuiObjectInfoSelectable selectable{ info };
      m_value.push_back(std::make_shared<GuiObjectInfoSelectable>(selectable));
      if (m_hide_points && dynamic_cast<PointInfo*>(selectable.get_value()))
      {
        selectable.get_value()->set_visibility(false);
      }
    }

    inline void handle_event(ObjectRemovedEvent& event)
    {
      auto found = std::find_if(m_value.begin(),
                                m_value.end(),
                                [&](const auto& item) { return item->get_label() == event.get_name(); });

      if (found != m_value.end()) { m_value.erase(found); }
    }

    inline void handle_event(GuiCheckboxChangedEvent& event)
    {
      m_hide_points = event.get_value();

      for (auto& selectable : m_value)
      {
        if (dynamic_cast<PointInfo*>(selectable->get_value()))
        {
          selectable->get_value()->set_visibility(!event.get_value());
        }
      }
    }
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_GUISELECTABLELISTBOX_HH
