using System;

using Sbx;

class Test : Behaviour {

  private float _speed = 1.5f;

  public override void OnCreate() {
    Logger.Info($"Test attached to {ToString()} with speed {_speed}");
  }

}

