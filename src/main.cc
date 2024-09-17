#include "CadRenderer.hh"
#include "ClientPoint.hh"
#include "Layers/CadLayer.hh"

class SandBoxApp : public esp::EspApplication
{
 public:
  SandBoxApp() :
      EspApplication({ .m_title          = "My window",
                       .m_width          = (uint32_t)mg1::WindowConstants::root_win_w,
                       .m_height         = (uint32_t)mg1::WindowConstants::root_win_h,
                       .m_disable_cursor = false,
                       .m_use_gui        = true })
  {
    mg1::CadRenderer::create();

    push_layer(new mg1::CadLayer());
  }
  ~SandBoxApp() override { mg1::CadRenderer::terminate(); }

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
