using System;
using Sbx.Managed.Interop;

namespace Sbx.Core
{

  public abstract class Component
  {
    public uint Node { get; internal set; }
  }

  public class Tag : Component
	{
		public string? Value
    {
      get { unsafe { return InternalCalls.Tag_GetTag(Node); } }
      set { unsafe { InternalCalls.Tag_SetTag(Node, value); } }
    }

    public override string ToString()
    {
      return Value ?? "[Unknown]";
    }
	}

} // namespace Sbx.Core