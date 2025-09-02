using System;

using Sbx;

class Test : Behaviour {

  private float speed = 1.5f;

  public override void OnCreate() {
    Logger.Info($"Test attached to {Entity.Name} with speed {speed}");
  }

}

