#ifndef ESPERT_SANDBOX_OBJECTLAYER_HH
#define ESPERT_SANDBOX_OBJECTLAYER_HH

#include "Espert.hh"
#include "Events/Gui/GuiEvents.hh"
#include "Events/Objects/ObjectEvents.hh"
#include "ObjectFactory.hh"
#include "ObjectSelector.hh"

using namespace esp;

namespace mg1
{
  class ObjectLayer : public Layer
  {
   private:
    Scene* m_scene;

    std::unique_ptr<ObjectSelector> m_object_selector;
    std::unique_ptr<ObjectFactory> m_object_factory;

    std::vector<std::unique_ptr<Layer>> m_children{};

   public:
    ObjectLayer(Scene* scene);

    virtual void pre_update(float dt) override;
    virtual void update(float dt) override;
    virtual void post_update(float dt) override;
    virtual void handle_event(Event& event, float dt) override;
  };
}; // namespace mg1

#endif // ESPERT_SANDBOX_OBJECTLAYER_HH
