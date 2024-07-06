using System;
using System.Collections.Generic;
using System.Text;
using System.Xml.Xsl;
using System.Xml;
using System.IO;

namespace Kotuku
{

  internal class Program
  {
    static string targetFilename;
    static string directoryPath;
    static string baseFilename;
    static string appName;

    static OptionSet options = new OptionSet()
      .Add("o=", "Optional output filename", delegate (string s) { targetFilename = s; })
      .Add("a=", "Application name", delegate (string s) { appName = s; })
      .Add("h|help", "Show this message and exit", delegate (string s) { ShowHelp(); });


    private static void ShowHelp()
    {
      Console.WriteLine("Usage: yehui.exe [OPTIONS]+ {xml-filename}");
      Console.WriteLine(" {xml-filename}.c   -- Application implementation");
      Console.WriteLine(" {xml-filename}.h   -- Implementation header");
      Console.WriteLine();

      // output the options
      Console.WriteLine("Options:");
      options.WriteOptionDescriptions(Console.Out);
    }

    static void Main(string[] args)
    {
      List<string> extras;
      try
      {
        extras = options.Parse(args);

        if (extras == null || extras.Count != 1)
        {
          Console.WriteLine("Error missing xml-filename  Usage: yehudi [Options] <xml-filename>");
          return;
        }

        string path = extras[0];
        string currentDirectory = Environment.CurrentDirectory;
        string absolutePath = Path.Combine(currentDirectory, path);
        Console.WriteLine("Processing: " + absolutePath);

        directoryPath = Path.GetDirectoryName(absolutePath);
        string sourceFilename = Path.GetFileName(absolutePath);

        baseFilename = Path.GetFileNameWithoutExtension(absolutePath);

        string appFilename = Path.Combine(directoryPath, baseFilename + ".c");
        string hdrFilename = Path.Combine(directoryPath, baseFilename + ".h");

        XslCompiledTransform xform = new XslCompiledTransform();

        Stream stream =
          System.Reflection.Assembly.GetAssembly(typeof(Program)).GetManifestResourceStream(
          "Kotuku.Yehudi.implemenation.xsl");

        xform.Load(new XmlTextReader(stream), null, new XmlUrlResolver());

        StringBuilder sb = new StringBuilder();

        XsltArgumentList transformArguments = new XsltArgumentList();
        transformArguments.AddParam("app-name", string.Empty, appName);

        using (StreamReader source = new StreamReader(sourceFilename))
        {
          using (StreamWriter writer = new StreamWriter(appFilename))
          {
            // transform the wsdl to a DOM
            xform.Transform(new XmlTextReader(source), transformArguments, new XmlTextWriter(writer));

            writer.Close();
          }

          source.Close();
        }

        stream =
          System.Reflection.Assembly.GetAssembly(typeof(Program)).GetManifestResourceStream(
          "Kotuku.Yehudi.definitions.xsl");

        xform.Load(new XmlTextReader(stream), null, new XmlUrlResolver());

        sb = new StringBuilder();

        using (StreamReader source = new StreamReader(sourceFilename))
        {
          using (StreamWriter writer = new StreamWriter(hdrFilename))
          {
            // transform the wsdl to a DOM
            xform.Transform(new XmlTextReader(source), transformArguments, new XmlTextWriter(writer));

            writer.Close();
          }

          source.Close();
        }

      }
      catch (OptionException e)
      {
        // output some error message
        Console.Write("yehudi: ");
        Console.WriteLine(e.Message);
        Console.WriteLine("Try `yehudi --help' for more information.");
        return;
      }

    }
  }
}