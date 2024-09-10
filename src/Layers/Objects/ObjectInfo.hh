#ifndef ESPERT_SANDBOX_OBJECTINFO_HH
#define ESPERT_SANDBOX_OBJECTINFO_HH

#include "Espert.hh"
#include "Utils/Enums.hh"
#include "Utils/Labels.hh"

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
} // namespace mg1

#endif // ESPERT_SANDBOX_OBJECTINFO_HH
