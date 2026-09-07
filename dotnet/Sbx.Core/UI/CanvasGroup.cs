using Sbx.Core;

namespace Sbx.Core.UI
{

  public class CanvasGroup : Component
  {

    public float Alpha
    {
      get { unsafe { float value; InternalCalls.CanvasGroup_GetAlpha(UUID, &value); return value; } }
      set { unsafe { InternalCalls.CanvasGroup_SetAlpha(UUID, value); } }
    }

    public bool Interactable
    {
      get { unsafe { return InternalCalls.CanvasGroup_GetInteractable(UUID); } }
      set { unsafe { InternalCalls.CanvasGroup_SetInteractable(UUID, value); } }
    }

    public bool BlocksRaycasts
    {
      get { unsafe { return InternalCalls.CanvasGroup_GetBlocksRaycasts(UUID); } }
      set { unsafe { InternalCalls.CanvasGroup_SetBlocksRaycasts(UUID, value); } }
    }

    public bool IgnoreParentGroups
    {
      get { unsafe { return InternalCalls.CanvasGroup_GetIgnoreParentGroups(UUID); } }
      set { unsafe { InternalCalls.CanvasGroup_SetIgnoreParentGroups(UUID, value); } }
    }

  } // class CanvasGroup

} // namespace Sbx.Core.UI
