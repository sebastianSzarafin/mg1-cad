#ifndef ESPERT_SANDBOX_ENUMS_HH
#define ESPERT_SANDBOX_ENUMS_HH

#include "Espert.hh"

namespace mg1
{
  enum MouseState
  {
    GuiNotCaptured,
    GuiCaptured
  };

  enum CameraType
  {
    Fps,
    Orbit
  };

  enum RotationAxis
  {
    RotationNone,
    RotationOX,
    RotationOY,
    RotationOZ
  };

  enum ScaleAxis
  {
    ScaleNone,
    Scale,
    ScaleOX,
    ScaleOY,
    ScaleOZ
  };

  enum SplineBase
  {
    Bernstein = 0,
    BSpline   = 1
  };

  enum class CursorType
  {
    Object = 0,
    Mouse  = 1
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_ENUMS_HH
