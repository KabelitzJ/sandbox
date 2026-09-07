using Sbx.Core;

namespace Sbx.Core.Components
{

  public class Tag : Component
  {

    public string? Value
    {
      get
      {
        unsafe { return InternalCalls.Tag_GetTag(UUID); }
      }
      set
      {
        unsafe { InternalCalls.Tag_SetTag(UUID, value); }
      }
    }

    public override string ToString()
    {
      return Value ?? "[Unknown]";
    }

  } // public class Tag

} // namespace Sbx.Core.Components
