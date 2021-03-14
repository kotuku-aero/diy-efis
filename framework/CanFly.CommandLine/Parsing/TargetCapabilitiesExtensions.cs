using CanFly.CommandLine.Infrastructure;

namespace CanFly.CommandLine.Parsing
{
  /// <summary>
  /// Utility extension methods for query target capabilities.
  /// </summary>
  internal static class TargetCapabilitiesExtensions
  {
    public static bool HasVerbs(object target)
    {
      return ReflectionHelper.RetrievePropertyList<VerbOptionAttribute>(target).Count > 0;
    }

    public static bool HasHelp(object target)
    {
      return ReflectionHelper.RetrieveMethod<HelpOptionAttribute>(target) != null;
    }

    public static bool HasVerbHelp(object target)
    {
      return ReflectionHelper.RetrieveMethod<HelpVerbOptionAttribute>(target) != null;
    }

    public static bool CanReceiveParserState(object target)
    {
      return ReflectionHelper.RetrievePropertyList<ParserStateAttribute>(target).Count > 0;
    }
  }
}