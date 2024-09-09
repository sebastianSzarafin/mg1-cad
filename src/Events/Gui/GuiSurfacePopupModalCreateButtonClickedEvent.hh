#ifndef ESPERT_SANDBOX_GUISURFACEPOPUPMODALCREATEBUTTONCLICKEDEVENT_HH
#define ESPERT_SANDBOX_GUISURFACEPOPUPMODALCREATEBUTTONCLICKEDEVENT_HH

#include "GuiEvent.hh"
#include "Utils/Enums.hh"

namespace mg1
{
  struct CreateSurfaceData
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
    CreateSurfaceData m_create_surface_data;

   public:
    GuiSurfacePopupModalCreateButtonClickedEvent(const std::string& label, CreateSurfaceData data) :
        GuiEvent(label), m_create_surface_data{ data }
    {
    }

    inline CreateSurfaceData get_data() { return m_create_surface_data; }
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_GUISURFACEPOPUPMODALCREATEBUTTONCLICKEDEVENT_HH
