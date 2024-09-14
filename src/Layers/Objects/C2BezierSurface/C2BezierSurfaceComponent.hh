#ifndef ESPERT_SANDBOX_C2BEZIERSURFACECOMPONENT_HH
#define ESPERT_SANDBOX_C2BEZIERSURFACECOMPONENT_HH

#include "Layers/Objects/C0BezierSurface/C0BezierSurfaceComponent.hh"

namespace mg1
{
  class C2BezierSurfaceComponent : public C0BezierSurfaceComponent
  {
   private:
    static const int s_patch_size   = 4;
    static const int s_patch_offset = 1;

   public:
    C2BezierSurfaceComponent(int id, Scene* scene, CreateSurfaceData data);
    ~C2BezierSurfaceComponent() = default;

    void handle_event(GuiCheckboxChangedEvent& event) override;
    void handle_event(CursorRotChangedEvent& event) override;
    void handle_event(CursorScaleChangedEvent& event) override;

   private:
    std::vector<uint32_t> create_control_points(CreateSurfaceData data) override;
    inline const int patch_size() override { return s_patch_size; }
    inline const int patch_offset() override { return s_patch_offset; }
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_C2BEZIERSURFACECOMPONENT_HH
