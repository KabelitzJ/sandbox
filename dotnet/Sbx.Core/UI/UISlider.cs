using Sbx.Core;

namespace Sbx.Core.UI
{

  public class UISlider : Component
  {

    public float Value
    {
      get { unsafe { float value; InternalCalls.UISlider_GetValue(UUID, &value); return value; } }
      set { unsafe { InternalCalls.UISlider_SetValue(UUID, value); } }
    }

    public float MinValue
    {
      get { unsafe { float value; InternalCalls.UISlider_GetMinValue(UUID, &value); return value; } }
      set { unsafe { InternalCalls.UISlider_SetMinValue(UUID, value); } }
    }

    public float MaxValue
    {
      get { unsafe { float value; InternalCalls.UISlider_GetMaxValue(UUID, &value); return value; } }
      set { unsafe { InternalCalls.UISlider_SetMaxValue(UUID, value); } }
    }

    public bool WholeNumbers
    {
      get { unsafe { return InternalCalls.UISlider_GetWholeNumbers(UUID); } }
      set { unsafe { InternalCalls.UISlider_SetWholeNumbers(UUID, value); } }
    }

    public bool Interactable
    {
      get { unsafe { return InternalCalls.UISlider_GetInteractable(UUID); } }
      set { unsafe { InternalCalls.UISlider_SetInteractable(UUID, value); } }
    }

  } // class UISlider

} // namespace Sbx.Core.UI
