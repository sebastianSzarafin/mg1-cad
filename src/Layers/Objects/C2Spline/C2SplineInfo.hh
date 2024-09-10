#ifndef ESPERT_SANDBOX_C2SPLINEINFO_HH
#define ESPERT_SANDBOX_C2SPLINEINFO_HH

#include "Layers/Gui/GuiFields/GuiRadioButtons.hh"
#include "Layers/Objects/C0Spline/C0SplineInfo.hh"

namespace mg1
{
  struct C2SplineInfo : public C0SplineInfo
  {
   private:
    std::shared_ptr<GuiRadioButtons> m_spline_base_radio_buttons;

   public:
    C2SplineInfo(uint32_t id, const std::string& name, std::vector<PointInfo*> control_points) :
        C0SplineInfo(id, name, control_points)
    {
      m_spline_base_radio_buttons =
          std::make_shared<GuiRadioButtons>(GuiLabel::m_spline_base_radio_buttons,
                                            0,
                                            std::vector<std::string>{ "Bernstein", "B-Spline" });
    }

    inline void render() override
    {
      C0SplineInfo::render();

      ImGui::Text("Spline base: ");
      m_spline_base_radio_buttons->render();
    }
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_C2SPLINEINFO_HH
