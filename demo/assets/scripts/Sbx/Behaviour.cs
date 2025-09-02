using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Sbx {

public abstract class Behaviour {

  protected Node _node;

  public override string ToString() => _node.ToString();

  public virtual void OnCreate() { }
  public virtual void OnUpdate(float delta_time) { }
  public virtual void OnDestroy() { }

  public bool HasComponent<T>() where T : unmanaged {
    return InternalHasComponent(_node, typeof(T));
  }

  public unsafe T GetComponent<T>() where T : unmanaged {
    T value = default;

    if (!InternalGetComponent(_node, typeof(T), (IntPtr)(&value), sizeof(T))) {
      throw new InvalidOperationException($"Component {typeof(T).FullName} not present on {_node.ToString()}");
    }

    return value;
  }

  [MethodImpl(MethodImplOptions.InternalCall)]
  internal static extern bool InternalHasComponent(Node node, Type componentType);

  [MethodImpl(MethodImplOptions.InternalCall)]
  internal static extern bool InternalGetComponent(Node node, Type componentType, IntPtr outData, int size);

}

} // namespace Sbx