namespace Sbx.Managed
{

  /// <summary>
  /// Implemented by a Sbx.Core reference type that a script field can hold and that the generic
  /// field marshaling in Object.cs/Marshalling.cs should carry across the native boundary as a raw
  /// ulong handle instead of a real object pointer. Sbx.Managed sits below Sbx.Core in the
  /// dependency graph and can't reference concrete types like Node directly -- this interface,
  /// which Sbx.Managed owns, is the entire contract instead: read the current instance's handle,
  /// or construct one back from a handle.
  /// </summary>
  public interface INativeHandle
  {
    ulong Handle { get; }

    static abstract object? FromHandle(ulong handle);
  }

}
