using System.Formats.Asn1;
using Sbx.Core;

namespace Demo
{

  public class Demo : Behavior
  {

    public void SayHello()
    {
      var tag = GetComponent<Tag>();

      Logger.Info("Hello {0}", HasComponent<Tag>());
      Logger.Info("Hello {0}", Node);

      Logger.Info("Hello from {0}", tag.Value);
    }
    
    public void SetTag(string value)
    {
      var tag = GetComponent<Tag>();

      tag.Value = value;
    }

  } // class Demo

} // namespace Demo

