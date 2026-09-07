using Sbx.Math;

namespace Sbx.Core
{

  public class UIScrollRect : Component
  {

    public Vector2 NormalizedPosition
    {
      get { unsafe { Vector2 value; InternalCalls.UIScrollRect_GetNormalizedPosition(UUID, &value); return value; } }
      set { unsafe { InternalCalls.UIScrollRect_SetNormalizedPosition(UUID, &value); } }
    }

    public bool Horizontal
    {
      get { unsafe { return InternalCalls.UIScrollRect_GetHorizontal(UUID); } }
      set { unsafe { InternalCalls.UIScrollRect_SetHorizontal(UUID, value); } }
    }

    public bool Vertical
    {
      get { unsafe { return InternalCalls.UIScrollRect_GetVertical(UUID); } }
      set { unsafe { InternalCalls.UIScrollRect_SetVertical(UUID, value); } }
    }

  } // class UIScrollRect

} // namespace Sbx.Core
