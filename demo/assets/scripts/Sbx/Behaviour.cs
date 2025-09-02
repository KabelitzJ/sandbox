using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Sbx {

public abstract class Behaviour {

  protected Entity Entity;

  public virtual void OnCreate() { }
  public virtual void OnUpdate(float delta_time) { }
  public virtual void OnDestroy() { }

  public bool HasComponent<T>() where T : unmanaged {
    return InternalHasComponent(Entity, typeof(T));
  }

  public unsafe T GetComponent<T>() where T : unmanaged {
    T value = default;

    if (!InternalGetComponent(Entity, typeof(T), (IntPtr)(&value), sizeof(T))) {
      throw new InvalidOperationException($"Component {typeof(T).FullName} not present on {Entity.ToString()}");
    }

    return value;
  }

  public unsafe bool AddComponent<T>(in T value) where T : unmanaged {
    fixed (T* ptr = &value)

    return InternalAddComponent(Entity, typeof(T), (IntPtr)ptr, sizeof(T));
  }

  [MethodImpl(MethodImplOptions.InternalCall)]
  internal static extern bool InternalHasComponent(Entity entity, Type componentType);

  [MethodImpl(MethodImplOptions.InternalCall)]
  internal static extern bool InternalGetComponent(Entity entity, Type componentType, IntPtr outData, int size);

  [MethodImpl(MethodImplOptions.InternalCall)]
  internal static extern bool InternalAddComponent(Entity entity, Type componentType, IntPtr inData, int size);
}

} // namespace Sbx