using Sbx.Core;

namespace Sbx.Core.UI
{

  public class UIScrollbar : Component
  {

    public float Value
    {
      get { unsafe { float value; InternalCalls.UIScrollbar_GetValue(UUID, &value); return value; } }
      set { unsafe { InternalCalls.UIScrollbar_SetValue(UUID, value); } }
    }

    public float Size
    {
      get { unsafe { float value; InternalCalls.UIScrollbar_GetSize(UUID, &value); return value; } }
      set { unsafe { InternalCalls.UIScrollbar_SetSize(UUID, value); } }
    }

    public bool Interactable
    {
      get { unsafe { return InternalCalls.UIScrollbar_GetInteractable(UUID); } }
      set { unsafe { InternalCalls.UIScrollbar_SetInteractable(UUID, value); } }
    }

  } // class UIScrollbar

} // namespace Sbx.Core.UI
