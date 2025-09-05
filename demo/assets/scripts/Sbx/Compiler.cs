using System;
using System.CodeDom.Compiler;
using System.IO;
using Microsoft.CSharp;

namespace Sbx {

public static class Compiler {

  // Returns true on success; 'log' contains diagnostics.
  public static bool CompileToDll(string[] sources, string outputPath, string[] references, bool debug, out string log) {
    using var provider = new CSharpCodeProvider();

    var parameters = new CompilerParameters{
      GenerateExecutable = false,
      OutputAssembly = outputPath,
      IncludeDebugInformation = debug,
      TreatWarningsAsErrors = false,
      WarningLevel = 4
    };

    parameters.ReferencedAssemblies.Add("System.dll");
    parameters.ReferencedAssemblies.Add("System.Core.dll");

    foreach (var reference in references) {
      if (!string.IsNullOrEmpty(reference)) {
        references.ReferencedAssemblies.Add(reference);
      }
    }

    var result = provider.CompileAssemblyFromFile(parameters, sources);

    using var sw = new StringWriter();
    
    foreach (var error in result.Errors) {
      sw.WriteLine($"{(e.IsWarning ? "warning" : "error")} {e.ErrorNumber}: {e.ErrorText} ({e.FileName}:{e.Line},{e.Column})");
    }

    log = sw.ToString();

    return !result.Errors.HasErrors;
  }
    
} // class Compiler

} // namespace Sbx
