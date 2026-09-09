using Sbx.Core;
using Sbx.Core.Math;

namespace Sbx.Core.Physics
{

  public enum NavAgentState
  {
    Idle,
    Moving,
    TargetUnreachable
  }

  public class NavAgent : Component
  {

    /** Finds a path over the baked navmesh and starts walking there -- see NavMesh.Bake. Returns false if there's no navmesh yet, or no path was found. */
    public bool SetDestination(Vector3 target)
    {
      unsafe { return InternalCalls.NavAgent_SetDestination(UUID, &target); }
    }

    public NavAgentState State
    {
      get { unsafe { return (NavAgentState)InternalCalls.NavAgent_GetState(UUID); } }
    }

    public Vector3 Velocity
    {
      get
      {
        Vector3 velocity;
        unsafe { InternalCalls.NavAgent_GetVelocity(UUID, &velocity); }
        return velocity;
      }
    }

    /** Straight-line distance from the agent's current position to its current destination. */
    public float RemainingDistance
    {
      get
      {
        float distance;
        unsafe { InternalCalls.NavAgent_GetRemainingDistance(UUID, &distance); }
        return distance;
      }
    }

    public float Radius
    {
      get
      {
        float radius;
        unsafe { InternalCalls.NavAgent_GetRadius(UUID, &radius); }
        return radius;
      }
      set
      {
        unsafe { InternalCalls.NavAgent_SetRadius(UUID, value); }
      }
    }

    public float Speed
    {
      get
      {
        float speed;
        unsafe { InternalCalls.NavAgent_GetSpeed(UUID, &speed); }
        return speed;
      }
      set
      {
        unsafe { InternalCalls.NavAgent_SetSpeed(UUID, value); }
      }
    }

    public float Acceleration
    {
      get
      {
        float acceleration;
        unsafe { InternalCalls.NavAgent_GetAcceleration(UUID, &acceleration); }
        return acceleration;
      }
      set
      {
        unsafe { InternalCalls.NavAgent_SetAcceleration(UUID, value); }
      }
    }

  } // class NavAgent

} // namespace Sbx.Core.Physics
