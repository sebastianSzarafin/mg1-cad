#ifndef ESPERT_SANDBOX_GUISURFACEPOPUPMODALCREATEBUTTONCLICKEDEVENT_HH
#define ESPERT_SANDBOX_GUISURFACEPOPUPMODALCREATEBUTTONCLICKEDEVENT_HH

#include "GuiEvent.hh"
#include "Utils/Enums.hh"

namespace mg1
{
  struct SurfaceComponentParams
  {
    SurfaceType m_type;
    SurfaceContinuity m_continuity;
    float m_width;
    float m_height;
    int m_segments_u;
    int m_segments_v;
  };

  class GuiSurfacePopupModalCreateButtonClickedEvent : public GuiEvent
  {
   private:
    SurfaceComponentParams m_surface_component_params;

   public:
    GuiSurfacePopupModalCreateButtonClickedEvent(const std::string& label, SurfaceComponentParams params) :
        GuiEvent(label), m_surface_component_params{ params }
    {
    }

    inline SurfaceComponentParams get_data() { return m_surface_component_params; }
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_GUISURFACEPOPUPMODALCREATEBUTTONCLICKEDEVENT_HH
