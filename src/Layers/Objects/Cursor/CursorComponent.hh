#ifndef ESPERT_SANDBOX_CURSORCOMPONENT_HH
#define ESPERT_SANDBOX_CURSORCOMPONENT_HH

#include "CursorInfo.hh"
#include "Espert.hh"
#include "Interfaces/Interfaces.hh"

using namespace esp;

namespace mg1
{
  class CursorComponent : public IComponent, public IEventable
  {
   private:
    std::shared_ptr<CursorInfo> m_info;
    glm::vec3 m_previous_position;
    bool m_visible;

   public:
    CursorComponent(entt::entity id, CursorType type, glm::vec3 position = { 0, 0, 0 });
    ~CursorComponent() = default;

    static std::tuple<std::vector<Vertex>, std::vector<uint32_t>> construct();

    inline CursorInfo* get_info() { return m_info.get(); }

    void update();
    void update_when_mouse_pressed();

    inline glm::vec3 get_position() { return TransformManager::get_translation(m_id); }
    inline glm::vec3 get_delta_position() { return get_position() - m_previous_position; }
    inline CursorType get_type() { return m_info->m_type; }
    inline bool is_type(CursorType type) { return m_info->m_type == type; }
    inline bool visible() const { return m_visible; }
    inline void show() { m_visible = true; }
    inline void hide() { m_visible = false; }

    void handle_event(MouseMovedEvent& event, float dt, RotationAxis rotation_axis);
    void handle_event(MouseScrolledEvent& event, ScaleAxis scale_axis);

   private:
    void update_mouse();
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_CURSORCOMPONENT_HH
