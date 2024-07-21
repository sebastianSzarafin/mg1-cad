#include "ObjectLayer.hh"
#include "MG1/Common/InitInfo.hh"

namespace mg1
{
  ObjectLayer::ObjectLayer(Scene* scene) : m_scene{ scene }
  {
    m_object_selector = ObjectSelector::create(m_scene);
    m_object_factory  = ObjectFactory::create(m_scene, m_object_selector.get());

    // create children layers
    {
      m_children.emplace_back(new TorusLayer(m_scene));
      m_children.emplace_back(new PointLayer(m_scene));
      m_children.emplace_back(new SplineLayer(m_scene));
      m_children.emplace_back(new C2SplineLayer(m_scene));
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
