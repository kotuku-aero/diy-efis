using System.Globalization;

namespace CanFly.CommandLine
{
    /// <summary>
    /// Utility extension methods for System.String.
    /// </summary>
    internal static class StringExtensions
    {
        public static string Spaces(int value)
        {
            return new string(' ', value);
        }

        public static bool IsNumeric(string value)
        {
            decimal temporary;
            return decimal.TryParse(value, out temporary);
        }

        public static string FormatInvariant(string value, params object[] arguments)
        {
            return string.Format(CultureInfo.InvariantCulture, value, arguments);
        }

        public static string FormatLocal(string value, params object[] arguments)
        {
            return string.Format(CultureInfo.CurrentCulture, value, arguments);
        }
    }
}