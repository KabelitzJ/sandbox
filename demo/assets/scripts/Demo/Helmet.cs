using System.Formats.Asn1;
using Sbx.Core;

namespace Demo
{

  public class Helmet : Behavior
  {

    public void SayHello()
    {
      var tag = GetComponent<Tag>();
      var transform = GetComponent<Transform>();

      Logger.Info("Hello {0}", HasComponent<Tag>());
      Logger.Info("Hello {0}", Node);

      Logger.Info("Hello from {0}", tag);
      Logger.Info("Position: {0}", transform.Position);

      transform.Position = new Vector3(4, 5, 6);
    }

    public void SetTag(string value)
    {
      var tag = GetComponent<Tag>();

      tag.Value = value;
    }
    
    protected override void OnUpdate(float deltaTime)
    {
      if (Input.IsKeyPressed(KeyCode.Space))
      {
        Logger.Info("Space pressed from {0}", GetComponent<Tag>());
      }
    }

  } // class Demo

} // namespace Demo

