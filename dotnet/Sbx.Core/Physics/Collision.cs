using Sbx.Core;
using Sbx.Core.Math;

namespace Sbx.Core.Physics
{

  public readonly struct Collision
  {

    public Node Other { get; }
    public Vector3 Normal { get; }
    public Vector3 Point { get; }

    public Collision(Node other, Vector3 normal, Vector3 point)
    {
      Other = other;
      Normal = normal;
      Point = point;
    }

  } // struct Collision

} // namespace Sbx.Core.Physics
