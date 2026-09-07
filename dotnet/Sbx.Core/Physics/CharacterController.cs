using Sbx.Core;
using Sbx.Core.Math;

namespace Sbx.Core.Physics
{

  [Flags]
  public enum CollisionFlags : byte
  {
    None  = 0,
    Below = 1 << 0,
    Above = 1 << 1,
    Sides = 1 << 2
  } // enum CollisionFlags

  public class CharacterController : Component
  {
    public float Height
    {
      get
      {
        float height;
        unsafe { InternalCalls.CharacterController_GetHeight(UUID, &height); }
        return height;
      }
    }

    public float Radius
    {
      get
      {
        float radius;
        unsafe { InternalCalls.CharacterController_GetRadius(UUID, &radius); }
        return radius;
      }
    }

    public float SlopeLimit
    {
      get
      {
        float slopeLimit;
        unsafe { InternalCalls.CharacterController_GetSlopeLimit(UUID, &slopeLimit); }
        return slopeLimit;
      }
    }

    public float StepOffset
    {
      get
      {
        float stepOffset;
        unsafe { InternalCalls.CharacterController_GetStepOffset(UUID, &stepOffset); }
        return stepOffset;
      }
    }

    public bool IsGrounded
    {
      get
      {
        unsafe { return InternalCalls.CharacterController_GetIsGrounded(UUID); }
      }
    }

    public CollisionFlags Flags
    {
      get
      {
        CollisionFlags flags;
        unsafe { InternalCalls.CharacterController_GetFlags(UUID, (byte*)&flags); }
        return flags;
      }
    }

    public void Move(Vector3 displacement)
    {
      unsafe { InternalCalls.CharacterController_Move(UUID, &displacement); }
    }
  }

} // namespace Sbx.Core.Physics
