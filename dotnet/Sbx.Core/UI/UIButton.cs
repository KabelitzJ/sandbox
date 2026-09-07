using Sbx.Core;
using Sbx.Core.Math;

namespace Sbx.Core.UI
{

  public class UIButton : Component
  {

    public bool Interactable
    {
      get { unsafe { return InternalCalls.UIButton_GetInteractable(UUID); } }
      set { unsafe { InternalCalls.UIButton_SetInteractable(UUID, value); } }
    }

    public Color NormalColor
    {
      get { unsafe { Color value; InternalCalls.UIButton_GetNormalColor(UUID, &value); return value; } }
      set { unsafe { InternalCalls.UIButton_SetNormalColor(UUID, &value); } }
    }

    public Color HoveredColor
    {
      get { unsafe { Color value; InternalCalls.UIButton_GetHoveredColor(UUID, &value); return value; } }
      set { unsafe { InternalCalls.UIButton_SetHoveredColor(UUID, &value); } }
    }

    public Color PressedColor
    {
      get { unsafe { Color value; InternalCalls.UIButton_GetPressedColor(UUID, &value); return value; } }
      set { unsafe { InternalCalls.UIButton_SetPressedColor(UUID, &value); } }
    }

    public bool IsHovered
    {
      get { unsafe { return InternalCalls.UIButton_GetIsHovered(UUID); } }
    }

    public bool IsPressed
    {
      get { unsafe { return InternalCalls.UIButton_GetIsPressed(UUID); } }
    }

    public bool WasClicked
    {
      get { unsafe { return InternalCalls.UIButton_GetWasClicked(UUID); } }
    }

  } // class UIButton

} // namespace Sbx.Core.UI
