#ifndef ESPERT_SANDBOX_OBJECTFACTORY_HH
#define ESPERT_SANDBOX_OBJECTFACTORY_HH

#include "Espert.hh"
#include "Events/Objects/ObjectRemovedEvent.hh"
#include "ObjectSelector.hh"

using namespace esp;

namespace mg1
{
  class GridComponent;
  class CursorComponent;
  class TorusComponent;
  class PointComponent;
  class C0SplineComponent;
  class C2SplineComponent;
  class C2InterpolationSplineComponent;
  class C0BezierSurfaceComponent;
  class C2BezierSurfaceComponent;
  struct GridComponentParams;
  struct SurfaceComponentParams;

  class ObjectFactory : IEventable
  {
   private:
    static ObjectFactory* s_instance;

    std::shared_ptr<EspShader> m_grid_shader;
    std::shared_ptr<EspShader> m_cursor_shader;
    std::shared_ptr<EspShader> m_object_shader;
    std::shared_ptr<EspShader> m_spline_shader;
    std::shared_ptr<EspShader> m_c0_surface_shader;
    std::shared_ptr<EspShader> m_c2_surface_shader;
    std::shared_ptr<EspShader> m_control_line_shader;

    Scene* m_scene;
    ObjectSelector* m_object_selector;

   public:
    ObjectFactory(Scene* scene, ObjectSelector* object_selector);
    ~ObjectFactory() = default;

    static std::unique_ptr<ObjectFactory> create(Scene* scene, ObjectSelector* object_selector);

    static GridComponent& create_grid(GridComponentParams params);
    static CursorComponent& create_cursor(CursorType type, glm::vec3 position = { 0, 0, 0 });
    static TorusComponent& create_torus(glm::vec3 position = { 0, 0, 0 });
    static PointComponent& create_point(glm::vec3 position = { 0, 0, 0 });
    static PointComponent& create_bernstein_point(glm::vec3 position = { 0, 0, 0 });
    static PointComponent& create_surface_point(glm::vec3 position = { 0, 0, 0 });
    static C0SplineComponent& create_c0_spline();
    static C2SplineComponent& create_c2_spline();
    static C2InterpolationSplineComponent& create_c2_interpolation_spline();
    static C0BezierSurfaceComponent& create_c0_bezier_surface(SurfaceComponentParams data,
                                                              glm::vec3 position = { 0, 0, 0 });
    static C2BezierSurfaceComponent& create_c2_bezier_surface(SurfaceComponentParams data,
                                                              glm::vec3 position = { 0, 0, 0 });

    static PointComponent& get_control_point(uint32_t id);

    // ---------------------------------------- TEMPLATE FUNCTIONS ----------------------------------------
    template<typename ObjComponent> static void remove_object(ObjComponent& obj)
    {
      auto& node  = NodeComponent::get_node(obj.get_id());
      auto parent = node.m_parent;

      s_instance->m_object_selector->try_deselect_node(node);

      EspJob::done_all_jobs();
      NodeComponent::get_node(parent).remove_child(node.m_handle);
      ObjectRemovedEvent obj_removed_event{ obj.get_info() };
      s_instance->post_event(obj_removed_event);
      s_instance->m_scene->destroy_entity(obj.get_id());
    }
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_OBJECTFACTORY_HH
