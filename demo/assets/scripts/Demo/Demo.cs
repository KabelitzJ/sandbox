using System.Formats.Asn1;
using Sbx.Core;

namespace Demo
{

  public class Demo : Behavior
  {

    public void SayHello()
    {
      Logger.Info("Hello {0}", HasComponent<Tag>());
      Logger.Info("Hello {0}", base.node);
      Logger.Info("Hello {0}", GetComponent<Tag>().Value);
    }

  } // class Demo

} // namespace Demo

