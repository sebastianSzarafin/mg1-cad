#include "ObjectLayer.hh"
#include "C0BezierSurface/C0BezierSurfaceLayer.hh"
#include "C0Spline/C0SplineLayer.hh"
#include "C2BezierSurface/C2BezierSurfaceLayer.hh"
#include "C2InterpolationSpline/C2InterpolationSplineLayer.hh"
#include "C2Spline/C2SplineLayer.hh"
#include "CoordinateSystemGrid/CoordinateSystemGridLayer.hh"
#include "Cursor/CursorLayer.hh"
#include "Point/PointLayer.hh"
#include "Torus/TorusLayer.hh"

namespace mg1
{
  ObjectLayer::ObjectLayer(Scene* scene) : m_scene{ scene }
  {
    m_object_selector = ObjectSelector::create(m_scene);
    m_object_factory  = ObjectFactory::create(m_scene, m_object_selector.get());

    // create children layers
    {
      m_children.emplace_back(new CursorLayer(m_scene));
      m_children.emplace_back(new CoordinateSystemGridLayer(m_scene));
      m_children.emplace_back(new TorusLayer(m_scene));
      m_children.emplace_back(new PointLayer(m_scene));
      m_children.emplace_back(new C0SplineLayer(m_scene));
      m_children.emplace_back(new C2SplineLayer(m_scene));
      m_children.emplace_back(new C2InterpolationSplineLayer(m_scene));
      m_children.emplace_back(new C0BezierSurfaceLayer(m_scene));
      m_children.emplace_back(new C2BezierSurfaceLayer(m_scene));
    }
  }

  void ObjectLayer::pre_update(float dt)
  {
    for (auto& child : m_children)
    {
      child->pre_update(dt);
    }

    m_object_selector->pre_update();
  }

  void ObjectLayer::update(float dt)
  {
    for (auto& child : m_children)
    {
      child->update(dt);
    }
  }

  void ObjectLayer::post_update(float dt)
  {
    for (auto& child : m_children)
    {
      child->post_update(dt);
    }
  }

  void ObjectLayer::handle_event(esp::Event& event, float dt)
  {
    for (auto& child : m_children)
    {
      child->handle_event(event, dt);
    }
  }
} // namespace mg1
