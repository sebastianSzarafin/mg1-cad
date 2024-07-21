#ifndef ESPERT_SANDBOX_OBJECTFACTORY_HH
#define ESPERT_SANDBOX_OBJECTFACTORY_HH

#include "Espert.hh"
#include "MG1/Components/Components.hh"
#include "ObjectSelector.hh"

using namespace esp;

namespace mg1
{
  class ObjectFactory : IEventable
  {
   private:
    static ObjectFactory* s_instance;

    std::shared_ptr<EspShader> m_object_shader;
    std::shared_ptr<EspShader> m_spline_shader;

    Scene* m_scene;
    ObjectSelector* m_object_selector;

   public:
    ObjectFactory(Scene* scene, ObjectSelector* object_selector);
    ~ObjectFactory() = default;

    static std::unique_ptr<ObjectFactory> create(Scene* scene, ObjectSelector* object_selector);

    static Entity* create_torus(glm::vec3 position = { 0, 0, 0 });
    static Entity* create_point(glm::vec3 position = { 0, 0, 0 });
    static Entity* create_spline();
    static Entity* create_c2_spline();

    // ---------------------------------------- TEMPLATE FUNCTIONS ----------------------------------------
    template<typename ObjComponent> static void remove_object(ObjComponent obj)
    {
      s_instance->m_object_selector->try_deselect_node(obj.get_node());

      EspJob::done_all_jobs();
      obj.get_node()->get_parent()->remove_child(obj.get_node());
      ObjectRemovedEvent obj_removed_event{ obj.get_info() };
      s_instance->post_event(obj_removed_event);
      s_instance->m_scene->destroy_entity(obj.get_info()->m_id);
    }
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_OBJECTFACTORY_HH
