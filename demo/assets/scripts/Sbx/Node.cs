using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Sbx {

  [StructLayout(LayoutKind.Sequential, Pack = 4)]
  public readonly struct Node : IEquatable<Node> {

  private readonly uint _id;

  internal uint Raw => _id;

  internal Node(uint id) => _id = id;

  public bool Equals(Node other) => _id == other._id;
  public override int GetHashCode() => (int)_id;
  public override string ToString() => InternalGetName(this);
  
  [MethodImpl(MethodImplOptions.InternalCall)]
  internal static extern string InternalGetName(Node node);

} // struct Node

} // namespace Sbx