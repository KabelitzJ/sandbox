using System;
using Sbx.Managed.Interop;
using Sbx.Core.Math;
using Sbx.Core.Physics;

namespace Sbx.Core
{
  public abstract class Behavior : Component
  {

    private static Dictionary<ulong, List<Behavior>> _behaviorRegistry = new Dictionary<ulong, List<Behavior>>();

    protected Behavior()
    {
      UUID = 0;
    }

    internal Behavior(ulong uuid)
    {
      UUID = uuid;
    }

    internal static void Register(Behavior behavior)
    {
      if (behavior.UUID == 0)
      {
        return;
      }

      if (!_behaviorRegistry.TryGetValue(behavior.UUID, out var list))
      {
        list = new List<Behavior>();
        _behaviorRegistry.Add(behavior.UUID, list);
      }

      list.Add(behavior);
    }

    internal static void Unregister(Behavior behavior)
    {
      if (!_behaviorRegistry.TryGetValue(behavior.UUID, out var list))
      {
        return;
      }

      list.Remove(behavior);

      if (list.Count == 0)
      {
        _behaviorRegistry.Remove(behavior.UUID);
      }
    }

    internal static T? GetBehavior<T>(ulong uuid) where T : Component
    {
      if (!_behaviorRegistry.TryGetValue(uuid, out var list))
      {
        return null;
      }

      foreach (var behavior in list)
      {
        if (behavior is T match)
        {
          return match;
        }
      }

      return null;
    }

    internal void DispatchOnCreate()
    {
      Register(this);
      OnCreate();
    }

    internal void DispatchOnDestroy()
    {
      Unregister(this);
      OnDestroy();
    }

    protected Node? Node
    {
      get
      {
        return Node.Get(UUID);
      }
    }

    public T? AddComponent<T>() where T : Component, new()
    {
      return Node?.AddComponent<T>();
    }

    public bool HasComponent<T>() where T : Component
    {
      return Node != null && Node.HasComponent<T>();
    }

    public bool HasComponent(Type type)
    {
      return Node != null && Node.HasComponent(type);
    }

    public T? GetComponent<T>() where T : Component, new()
    {
      var componentType = typeof(T);

      if (typeof(Behavior).IsAssignableFrom(componentType))
      {
        return GetBehavior<T>(UUID);
      }

      return Node?.GetComponent<T>();
    }

    public virtual void OnCreate()
    {
      
    }

    public virtual void OnUpdate()
    {
      
    }

    public virtual void OnFixedUpdate()
    {
      
    }

    public virtual void OnDestroy()
    {
      
    }

    public virtual void OnCollisionEnter(Collision collision)
    {
      
    }

    public virtual void OnCollisionExit(Collision collision)
    {
      
    }

    public virtual void OnTriggerEnter(Collision collision)
    {
      
    }

    public virtual void OnTriggerExit(Collision collision)
    {
    }

    public virtual void OnClick()
    {
      
    }

    public virtual void OnValueChanged()
    {

    }

    private void DispatchCollisionEnter(ulong otherUuid, float normalX, float normalY, float normalZ, float pointX, float pointY, float pointZ)
    {
      OnCollisionEnter(new Collision(Node.Get(otherUuid)!, new Vector3(normalX, normalY, normalZ), new Vector3(pointX, pointY, pointZ)));
    }

    private void DispatchCollisionExit(ulong otherUuid, float normalX, float normalY, float normalZ, float pointX, float pointY, float pointZ)
    {
      OnCollisionExit(new Collision(Node.Get(otherUuid)!, new Vector3(normalX, normalY, normalZ), new Vector3(pointX, pointY, pointZ)));
    }

    private void DispatchTriggerEnter(ulong otherUuid, float normalX, float normalY, float normalZ, float pointX, float pointY, float pointZ)
    {
      OnTriggerEnter(new Collision(Node.Get(otherUuid)!, new Vector3(normalX, normalY, normalZ), new Vector3(pointX, pointY, pointZ)));
    }

    private void DispatchTriggerExit(ulong otherUuid, float normalX, float normalY, float normalZ, float pointX, float pointY, float pointZ)
    {
      OnTriggerExit(new Collision(Node.Get(otherUuid)!, new Vector3(normalX, normalY, normalZ), new Vector3(pointX, pointY, pointZ)));
    }

  } // class Behavior

} // namespace Sbx.Core

