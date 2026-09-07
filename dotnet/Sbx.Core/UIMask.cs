namespace Sbx.Core
{

  public class UIMask : Component
  {

    public bool ShowMaskGraphic
    {
      get { unsafe { return InternalCalls.UIMask_GetShowMaskGraphic(UUID); } }
      set { unsafe { InternalCalls.UIMask_SetShowMaskGraphic(UUID, value); } }
    }

  } // class UIMask

} // namespace Sbx.Core
