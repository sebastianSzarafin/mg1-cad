#ifndef ESPERT_SANDBOX_CURSORINFO_HH
#define ESPERT_SANDBOX_CURSORINFO_HH

#include "Layers/Objects/ObjectInfo.hh"

namespace mg1
{
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

#endif // ESPERT_SANDBOX_CURSORINFO_HH
