namespace Sbx.Core
{

  public sealed class Node : Sbx.Managed.INativeHandle
  {
    private static Dictionary<ulong, Node> _nodeCache = new Dictionary<ulong, Node>();

    private Dictionary<Type, Component> _componentCache = new Dictionary<Type, Component>();

    private ulong _uuid { get; }

    /// <summary>This node's underlying scenes::id uuid — public since Component.UUID already is (see AddComponent).</summary>
    public ulong UUID => _uuid;

    /// <summary>Explicit -- INativeHandle is only how Sbx.Managed's generic field marshaling reads a Node field, not part of Node's own public API (that's UUID above).</summary>
    ulong Sbx.Managed.INativeHandle.Handle => _uuid;

    internal Node(ulong uuid)
    {
      _uuid = uuid;
    }

    internal static Node? Get(ulong uuid)
    {
      if (uuid == 0)
      {
        return null;
      }

      if (!_nodeCache.TryGetValue(uuid, out var node))
      {
        node = new Node(uuid);

        _nodeCache.Add(uuid, node);
      }

      return node;
    }

    /// <summary>
    /// INativeHandle's static factory, implemented implicitly (a plain public static method, not
    /// an explicit interface implementation) -- Sbx.Managed's SetFieldValue finds this by
    /// Type.GetMethod("FromHandle", ...) on the field's runtime type, not through the interface's
    /// static-abstract dispatch (that path needs a compile-time generic type parameter, which
    /// Sbx.Managed's marshaling code doesn't have here). Delegates to Get so the _nodeCache
    /// identity/reuse behavior is unchanged from every other way of obtaining a Node.
    /// </summary>
    public static object? FromHandle(ulong handle) => Get(handle);

    public string? Name
    {
      get
      {
        unsafe { return InternalCalls.Tag_GetTag(_uuid); }
      }
      set
      {
        unsafe { InternalCalls.Tag_SetTag(_uuid, value); }
      }
    }

    public T? GetComponent<T>() where T : Component, new()
    {
      var componentType = typeof(T);

      if (typeof(Behavior).IsAssignableFrom(componentType))
      {
        return Behavior.GetBehavior<T>(_uuid);
      }

      if (!HasComponent<T>())
      {
        _componentCache.Remove(componentType);
        return null;
      }

      if (!_componentCache.TryGetValue(componentType, out var component))
      {
        component = new T { UUID = _uuid };
        _componentCache.Add(componentType, component);
      }

      return component as T;
    }

    public bool HasComponent<T>() where T : Component
    {
      unsafe
      {
        return InternalCalls.Behavior_HasComponent(_uuid, typeof(T));
      }
    }

    public bool HasComponent(Type type)
    {
      unsafe
      {
        return InternalCalls.Behavior_HasComponent(_uuid, type);
      }
    }

    public T? AddComponent<T>() where T : Component, new()
    {
      var componentType = typeof(T);

      if (typeof(Behavior).IsAssignableFrom(componentType))
      {
        unsafe
        {
          InternalCalls.Scripting_AttachScript(_uuid, componentType.FullName);
        }

        return Behavior.GetBehavior<T>(_uuid);
      }

      if (HasComponent<T>())
      {
        return GetComponent<T>();
      }

      unsafe
      {
        InternalCalls.Behavior_AddComponent(_uuid, componentType);
      }

      var component = new T { UUID = _uuid };
      _componentCache.Add(componentType, component);

      return component;
    }

    public bool RemoveComponent<T>() where T : Component
		{
			var componentType = typeof(T);
      var removed = false;

			unsafe { removed = InternalCalls.Behavior_RemoveComponent(_uuid, componentType); }

			if (removed && _componentCache.ContainsKey(componentType))
      {
				_componentCache.Remove(componentType);
      }

			return removed;
		}

    public void Destroy()
    {
      unsafe
      {
        InternalCalls.Node_Destroy(_uuid);
      }

      _nodeCache.Remove(_uuid);
    }

    public void SetParent(Node? parent)
    {
      unsafe
      {
        InternalCalls.Node_SetParent(_uuid, parent?._uuid ?? 0);
      }
    }

    public static Node? Find(string name)
    {
      ulong uuid;

      unsafe
      {
        uuid = InternalCalls.Node_FindByName(name);
      }

      return Get(uuid);
    }

    public static Node? Create(string name)
    {
      ulong uuid;

      unsafe
      {
        uuid = InternalCalls.Node_Create(name);
      }

      return Get(uuid);
    }

    /// <summary>
    /// Instantiates the prefab at <paramref name="path"/> (project-relative, same convention as
    /// e.g. ParticleEffect.Load) as a new node subtree, optionally parented under
    /// <paramref name="parent"/>. Returns null if the path doesn't resolve to a valid prefab.
    /// </summary>
    public static Node? Instantiate(string path, Node? parent = null)
    {
      ulong uuid;

      unsafe
      {
        uuid = InternalCalls.Node_InstantiatePrefab(path, parent?._uuid ?? 0);
      }

      return Get(uuid);
    }

  } // class Node

} // namespace Sbx.Core
