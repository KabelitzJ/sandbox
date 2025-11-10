using System;
using System.Numerics;
using System.Runtime.InteropServices;

namespace Sbx.Core
{

  [StructLayout(LayoutKind.Sequential, Pack = 4)]
  public struct Vector3 : IEquatable<Vector3>
  {

    public static Vector3 Zero = new Vector3(0, 0, 0);
    public static Vector3 One = new Vector3(1, 1, 1);
    public static Vector3 Forward = new Vector3(0, 0, -1);
    public static Vector3 Back = new Vector3(0, 0, 1);
    public static Vector3 Right = new Vector3(1, 0, 0);
    public static Vector3 Left = new Vector3(-1, 0, 0);
    public static Vector3 Up = new Vector3(0, 1, 0);
    public static Vector3 Down = new Vector3(0, -1, 0);

    public static Vector3 Inifinity = new Vector3(float.PositiveInfinity);

    public float X;
    public float Y;
    public float Z;

    public Vector3(float x, float y, float z)
    {
      X = x;
      Y = y;
      Z = z;
    }

    public Vector3(float scalar)
    : this(scalar, scalar, scalar)
    { }

    public Vector3(float x, Vector2 yz)
    : this(x, yz.X, yz.Y)
    { }

    public Vector3(Vector2 xy, float z)
    : this(xy.X, xy.Y, z)
    { }

    public static Vector3 operator *(Vector3 left, float scalar)
    {
      return new Vector3(left.X * scalar, left.Y * scalar, left.Z * scalar);
    }

    public static Vector3 operator *(float scalar, Vector3 right)
    {
      return new Vector3(scalar * right.X, scalar * right.Y, scalar * right.Z);
    }

    public static Vector3 operator *(Vector3 left, Vector3 right)
    {
      return new Vector3(left.X * right.X, left.Y * right.Y, left.Z * right.Z);
    }

		public static Vector3 operator /(Vector3 left, Vector3 right) 
    {
      return new Vector3(left.X / right.X, left.Y / right.Y, left.Z / right.Z);
    }
    
		public static Vector3 operator /(Vector3 left, float scalar) 
    {
      return new Vector3(left.X / scalar, left.Y / scalar, left.Z / scalar);
    }
    
		public static Vector3 operator /(float scalar, Vector3 right) 
    {
      return new Vector3(scalar/ right.X, scalar/ right.Y, scalar/ right.Z);
    }
    
		public static Vector3 operator +(Vector3 left, Vector3 right) 
    {
      return new Vector3(left.X + right.X, left.Y + right.Y, left.Z + right.Z);
    }
    
		public static Vector3 operator +(Vector3 left, float right) 
    {
      return new Vector3(left.X + right, left.Y + right, left.Z + right);
    }
    
		public static Vector3 operator -(Vector3 left, Vector3 right) 
    {
      return new Vector3(left.X - right.X, left.Y - right.Y, left.Z - right.Z);
    }
    
		public static Vector3 operator -(Vector3 left, float right) 
    {
      return new Vector3(left.X - right, left.Y - right, left.Z - right);
    }

    public static Vector3 operator -(Vector3 vector)
    {
      return new Vector3(-vector.X, -vector.Y, -vector.Z);
    }

    public override bool Equals(object obj)
    {
      return obj is Vector3 other && Equals(other);
    }

    public bool Equals(Vector3 other) {
      return X == other.X && Y == other.Y && Z == other.Z;
    }

    public override int GetHashCode()
    {
      return (X, Y, Z).GetHashCode();
    }

    public static bool operator ==(Vector3 left, Vector3 right)
    {
      return left.Equals(right);
    }

    public static bool operator !=(Vector3 left, Vector3 right)
    {
      return !(left == right);
    }

    public float Length()
    {
      return (float)Math.Sqrt(X * X + Y * Y + Z * Z);
    }

    public static Vector3 Cross(Vector3 x, Vector3 y)
		{
			return new Vector3(
				x.Y * y.Z - y.Y * x.Z,
				x.Z * y.X - y.Z * x.X,
				x.X * y.Y - y.X * x.Y
			);
		}

    public static Vector3 Cos(Vector3 vector)
    {
      return Create(vector, Math.Cos);
    }

    public static Vector3 Sin(Vector3 vector)
    {
      return Create(vector, Math.Sin);
    }

    public override string ToString()
    {
      return string.Format("Vector3[{0}, {1}, {2}]", X, Y, Z);
    }

    public void Apply(Func<double, double> function)
		{
			X = (float)function(X);
			Y = (float)function(Y);
			Z = (float)function(Z);
		}

    public static Vector3 Create(Vector3 vector, Func<double, double> function)
    {
      return new Vector3((float)function(vector.X), (float)function(vector.Y), (float)function(vector.Z));
    }

  } // struct Vector3

} // namespace Sbx.Core