using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

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
  } // public class Tag

	public class Transform : Component
	{
    public Vector3 Position
    {
      get {
        Vector3 position;
        unsafe { InternalCalls.Transform_GetPosition(Node, &position); }
        return position;
      }
      set { unsafe { InternalCalls.Transform_SetPosition(Node, &value); } }
    }

		public Vector3 Rotation;
		public Vector3 Scale;

    public Vector3 Up
    {
      get => new Quaternion(Rotation) * Vector3.Up;
    }
    
		public Vector3 Right {
      get => new Quaternion(Rotation) * Vector3.Right; 
    }

		public Vector3 Forward {
      get => new Quaternion(Rotation) * Vector3.Forward; 
    }

		// public Transform(Vector3 position, Vector3 rotation, Vector3 scale)
		// {
		// 	Position = position;
		// 	Rotation = rotation;
		// 	Scale = scale;
		// }

    public override bool Equals(object? obj) {
      return obj is Transform other && Equals(other);
    }

		public bool Equals(Transform other)     {
      return Position == other.Position && Rotation == other.Rotation && Scale == other.Scale;
    }
    
		public override int GetHashCode()     {
      return (Position, Rotation, Scale).GetHashCode();
    }
    
		public static bool operator ==(Transform left, Transform right)     {
      return left.Equals(right);
    }
    
		public static bool operator !=(Transform left, Transform right)     {
      return !(left == right);
    }
    

	} // struct Transform

} // namespace Sbx.Core