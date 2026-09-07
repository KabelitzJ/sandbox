using Sbx.Math;

namespace Sbx.Core
{

  public class UIText : Component
  {

    public string? Text
    {
      get { unsafe { return InternalCalls.UIText_GetText(UUID); } }
      set { unsafe { InternalCalls.UIText_SetText(UUID, value); } }
    }

    public float FontSize
    {
      get { unsafe { float value; InternalCalls.UIText_GetFontSize(UUID, &value); return value; } }
      set { unsafe { InternalCalls.UIText_SetFontSize(UUID, value); } }
    }

    public Color Color
    {
      get { unsafe { Color value; InternalCalls.UIText_GetColor(UUID, &value); return value; } }
      set { unsafe { InternalCalls.UIText_SetColor(UUID, &value); } }
    }

  } // class UIText

} // namespace Sbx.Core
