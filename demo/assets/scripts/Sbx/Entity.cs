using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Sbx {

  [StructLayout(LayoutKind.Sequential)]
  public readonly struct Entity : IEquatable<Entity> {

  private readonly uint _id;

  internal uint Raw => _id;

  internal Entity(uint id) => _id = id;

  public string Name {
    get => InternalGetName(this);
  }

  public bool Equals(Entity other) => _id == other._id;
  public override int GetHashCode() => (int)_id;
  public override string ToString() => $"Entity({_id})";
  
  [MethodImpl(MethodImplOptions.InternalCall)]
  internal static extern string InternalGetName(Entity entity);

} // struct Entity

} // namespace Sbx {