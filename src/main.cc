#include "Examples/Examples.hh"
#include "Layers/Layers.hh"

#include "ClientPoint.hh"

class SandBoxApp : public esp::EspApplication
{
 private:
 public:
  SandBoxApp() : EspApplication("Espert Example", 1280, 720, true)
  {
    push_layer(new esp_sbx::InputLayer());

    /* ----------- Select 1 example ----------- */
    // push_layer(new my_game::ExampleLayer());
    // push_layer(new my_game::InstancingExampleLayer());
    // push_layer(new my_game::TextureExampleLayer());
    // push_layer(new my_game::TextureDepthExampleLayer());
    // push_layer(new model_example::ModelExampleLayer());
    // push_layer(new obj_example::VikingRoomObjModelExampleLayer());
    // push_layer(new obj_example::BackpackObjModelExampleLayer());
    push_layer(new advance_rendering_example::SkyBoxLayer());
    // push_layer(new advance_rendering_example::PBRBaseLayer2());
    //  push_layer(new advance_rendering_example::OffscreenRenderingLayer());
  }

  void virtual update(float dt) override
  {
    if (EspInput::is_key_pressed(ESP_KEY_ESCAPE)) { m_running = false; }
  }
};

esp::EspApplication* esp::create_app_instance()
{
  const auto& app = new SandBoxApp();
  return app;
}
