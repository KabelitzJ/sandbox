namespace Sbx.Core
{

  public class UIToggle : Component
  {

    public bool IsOn
    {
      get { unsafe { return InternalCalls.UIToggle_GetIsOn(UUID); } }
      set { unsafe { InternalCalls.UIToggle_SetIsOn(UUID, value); } }
    }

    public bool Interactable
    {
      get { unsafe { return InternalCalls.UIToggle_GetInteractable(UUID); } }
      set { unsafe { InternalCalls.UIToggle_SetInteractable(UUID, value); } }
    }

  } // class UIToggle

} // namespace Sbx.Core
