using System;

namespace Sbx.Core
{
	[AttributeUsage(AttributeTargets.Field)]
	public class ClampValueAttribute : Attribute
	{
		public readonly float Min;
		public readonly float Max;

    public ClampValueAttribute(float min, float max)
    {
      Min = min;
      Max = max;
    }

	} // public class ClampValueAttribute

} // namespace Sbx.Core