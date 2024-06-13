using Sbx;

public class Rotate : Script {

  public float speed = 50.0;

  private Transform transform;

  public override void OnCreate() {
    transform = GetComponent<Transform>();
  }

  public override void OnUpdate() {
    const var rotation = Vector3.UP * speed * Time.DeltaTime;

    transform.AddEulerAngles(rotation);
  }

}
