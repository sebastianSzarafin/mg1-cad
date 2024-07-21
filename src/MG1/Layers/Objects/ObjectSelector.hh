#ifndef ESPERT_SANDBOX_OBJECTSELECTOR_HH
#define ESPERT_SANDBOX_OBJECTSELECTOR_HH

#include "Espert.hh"
#include "MG1/Components/Components.hh"

using namespace esp;

namespace mg1
{
  class ObjectSelector : IEventable
  {
   private:
    Scene* m_scene;

    struct
    {
      std::vector<Node*> m_nodes{};
      glm::vec3 m_mass_centre{ 0, 0, 0 };

      std::vector<Node*>::iterator find(Node* node)
      {
        return std::find_if(m_nodes.begin(), m_nodes.end(), [&](const auto& item) { return item == node; });
      }

      bool try_add(Node* node)
      {
        auto it = find(node);
        if (it == m_nodes.end())
        {
          m_nodes.push_back(node);
          return true;
        }
        return false;
      }
      bool try_remove(Node* node)
      {
        auto it = find(node);
        if (it != m_nodes.end())
        {
          m_nodes.erase(it);
          return true;
        }
        return false;
      }
    } m_selected;

   public:
    ObjectSelector(Scene* scene);
    ~ObjectSelector() = default;

    static std::unique_ptr<ObjectSelector> create(Scene* scene);

    void pre_update();

    void try_select_node(Node* node);
    void try_deselect_node(Node* node);
    void update_mass_centre();
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_OBJECTSELECTOR_HH
