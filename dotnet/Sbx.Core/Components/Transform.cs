using Sbx.Core;
using Sbx.Core.Math;

namespace Sbx.Core.Components
{

	public class Transform : Component
	{

    public Vector3 Position
    {
      get
      {
        Vector3 position;
        unsafe { InternalCalls.Transform_GetPosition(UUID, &position); }
        return position;
      }
      set
      {
        unsafe { InternalCalls.Transform_SetPosition(UUID, &value); }
      }
    }

    public Vector3 WorldPosition
    {
      get
      {
        Vector3 position;
        unsafe { InternalCalls.Transform_GetWorldPosition(UUID, &position); }
        return position;
      }
    }

    public Quaternion Rotation
    {
      get
      {
        Quaternion rotation;
        unsafe { InternalCalls.Transform_GetRotation(UUID, &rotation); }
        return rotation;
      }
      set
      {
        unsafe { InternalCalls.Transform_SetRotation(UUID, &value); }
      }
    }

    public Vector3 Right
    {
      get
      {
        Vector3 right;
        unsafe { InternalCalls.Transform_GetRight(UUID, &right); }
        return right;
      }
    }

    public Vector3 Forward
    {
      get
      {
        Vector3 forward;
        unsafe { InternalCalls.Transform_GetForward(UUID, &forward); }
        return forward;
      }
    }

    public Vector3 Up
    {
      get
      {
        Vector3 up;
        unsafe { InternalCalls.Transform_GetUp(UUID, &up); }
        return up;
      }
    }

    public Vector3 Scale
    {
      get
      {
        Vector3 scale;
        unsafe { InternalCalls.Transform_GetScale(UUID, &scale); }
        return scale;
      }
      set
      {
        unsafe { InternalCalls.Transform_SetScale(UUID, &value); }
      }
    }

    public void LookAt(Vector3 target)
    {
      unsafe { InternalCalls.Transform_LookAt(UUID, &target); }
    }

    public override bool Equals(object? obj) {
      return obj is Transform other && Equals(other);
    }

		// Null-checked -- this is what "transform != null" (a plain reference null-check, not a value
		// comparison) actually calls via operator!= below, and Transform is a reference type (a
		// Component subclass), so a null "other" here is an expected, ordinary case, not a bug.
		public bool Equals(Transform? other)     {
      if (other is null) { return false; }
      return Position == other.Position && Rotation == other.Rotation && Scale == other.Scale;
    }

		public override int GetHashCode()     {
      return (Position, Rotation, Scale).GetHashCode();
    }

		public static bool operator ==(Transform? left, Transform? right)     {
      if (ReferenceEquals(left, right)) { return true; }
      if (left is null || right is null) { return false; }
      return left.Equals(right);
    }

		public static bool operator !=(Transform? left, Transform? right)     {
      return !(left == right);
    }

	} // class Transform

} // namespace Sbx.Core.Components
