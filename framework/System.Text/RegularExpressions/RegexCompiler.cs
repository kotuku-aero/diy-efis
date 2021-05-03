/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * Ported to C# for the .Net Micro Framework by <a href="mailto:juliusfriedman@gmail.com">Julius Friedman</a>
 * http://netmf.codeplex.com/
 * 
 * A regular expression compiler class.  This class compiles a pattern string into a
 * regular expression program interpretable by the RE evaluator class (Regexp).  The 'recompile'
 * command line tool uses this compiler to pre-compile regular expressions for use
 * with RE.  For a description of the syntax accepted by RECompiler and what you can
 * do with regular expressions, see the documentation for the RE (Regexp) matcher class.
 *
 * @see RE
 * @see recompile
 *
 * @author <a href="mailto:jonl@muppetlabs.com">Jonathan Locke</a>
 * @author <a href="mailto:gholam@xtra.co.nz">Michael McCallum</a>
 * @version $Id: RECompiler.java 518156 2007-03-14 14:31:26Z vgritsenko $
 * 
 * Adjusted by Laurent Ellerbach laurent@ellerbach.net
 * 2021/03/13
 */

using System.Collections;

namespace System.Text.RegularExpressions
{
  /// <summary>
  /// A regular expression compiler class.  This class compiles a pattern string into a
  /// regular expression program interpretable by the RE evaluator class.  The 'recompile'
  /// command line tool uses this compiler to pre-compile regular expressions for use
  /// with RE.  For a description of the syntax accepted by RECompiler and what you can
  /// do with regular expressions, see the documentation for the RE matcher class.
  /// </summary>
  public class RegexCompiler
  {

    // The compiled RE 'program' instruction buffer
    private char[] _instruction;
    // The amount of the program buffer currently in use
    private int _lenInstruction;


    // Input state for compiling regular expression
    private string _pattern;   // Input string
    private int _len;          // Length of the pattern string
    private int _idx;          // Current input index into ac
    private int _parens;       // Total number of paren pairs


    // Node flags
    private const int NODE_NORMAL = 0;     // No flags (nothing special)
    private const int NODE_NULLABLE = 1;   // True if node is potentially null
    private const int NODE_TOPLEVEL = 2;   // True if top level expr


    // {m,n} stacks
    private static int _bracketUnbounded = -1;  // Unbounded value
    private int _bracketMin;                    // Minimum number of matches
    private int _bracketOpt;                    // Additional optional matches


    /// <summary>
    /// Lookup table for POSIX char class names
    /// </summary>
    private static readonly Hashtable hashPOSIX = new Hashtable();
    static RegexCompiler()
    {
      hashPOSIX.Add("alnum", POSIXCharacterClass.Alphanumeric);
      hashPOSIX.Add("alpha", POSIXCharacterClass.Alphabetica);
      hashPOSIX.Add("blank", POSIXCharacterClass.Blank);
      hashPOSIX.Add("cntrl", POSIXCharacterClass.Control);
      hashPOSIX.Add("digit", POSIXCharacterClass.Digit);
      hashPOSIX.Add("graph", POSIXCharacterClass.GraphicCharacter);
      hashPOSIX.Add("lower", POSIXCharacterClass.LowerCase);
      hashPOSIX.Add("print", POSIXCharacterClass.Printable);
      hashPOSIX.Add("punct", POSIXCharacterClass.Punctuation);
      hashPOSIX.Add("space", POSIXCharacterClass.Spaces);
      hashPOSIX.Add("upper", POSIXCharacterClass.UpperCase);
      hashPOSIX.Add("xdigit", POSIXCharacterClass.Digit);
      hashPOSIX.Add("javastart", POSIXCharacterClass.JavaIdentifierStart);
      hashPOSIX.Add("javapart", POSIXCharacterClass.JavaIdentifierPart);


    }
    /// <summary>
    /// Gets the instructions.
    /// </summary>
    public char[] Instructions
    {
      get
      {
        return _instruction;
      }
    }

    /// <summary>
    /// Gets the pattern
    /// </summary>
    public string Pattern
    {
      get
      {
        return _pattern;
      }
    }

    /// <summary>
    /// Constructor.  Creates (initially empty) storage for a regular expression program.
    /// </summary>
    public RegexCompiler()
    {
      // Start off with a generous, yet reasonable, initial size
      _instruction = new char[128];
      _lenInstruction = 0;
    }

    /// <summary>
    /// Ensures that n more characters can fit in the program buffer.
    /// If n more can't fit, then the size is doubled until it can.
    /// </summary>
    /// <param name="n">Number of additional characters to ensure will fit.</param>
    void Ensure(int n)
    {
      // Get current program Length
      int curlen = _instruction.Length;

      // If the current Length + n more is too much
      if (_lenInstruction + n >= curlen)
      {
        // Double the size of the program array until n more will fit
        while (_lenInstruction + n >= curlen)
        {
          curlen *= 2;
        }

        // Allocate new program array and move data into it
        char[] newInstruction = new char[curlen];
        Array.Copy(_instruction, 0, newInstruction, 0, _lenInstruction);
        _instruction = newInstruction;
      }
    }

    /// <summary>
    /// Emit a single char into the program stream.
    /// </summary>
    /// <param name="c">char to add</param>
    void Emit(char c)
    {
      // Make room for character
      Ensure(1);

      // Add character
      _instruction[_lenInstruction++] = c;
    }

    /// <summary>
    /// Inserts a node with a given opcode and opdata at insertAt.  The node relative next
    /// pointer is initialized to 0.
    /// </summary>
    /// <param name="opcode">opcode Opcode for new node</param>
    /// <param name="opdata">opdata Opdata for new node (only the low 16 bits are currently used)</param>
    /// <param name="insertAt">insertAt Index at which to insert the new node in the program</param>
    void InsertNode(char opcode, int opdata, int insertAt)
    {
      // Make room for a new node
      Ensure(Regex.nodeSize);

      // Move everything from insertAt to the end down nodeSize elements
      Array.Copy(_instruction, insertAt, _instruction, insertAt + Regex.nodeSize, _lenInstruction - insertAt);
      _instruction[insertAt /* + RE.offsetOpcode */] = opcode;
      _instruction[insertAt + Regex.offsetOpdata] = (char)opdata;
      _instruction[insertAt + Regex.offsetNext] = CharacterClass.DefaultChar;
      _lenInstruction += Regex.nodeSize;
    }

    /// <summary>
    /// Appends a node to the end of a node chain
    /// </summary>
    /// <param name="node">Start of node chain to traverse</param>
    /// <param name="pointTo">Node to have the tail of the chain point to</param>
    void SetNextOfEnd(int node, int pointTo)
    {
      // Traverse the chain until the next offset is 0
      int next = _instruction[node + Regex.offsetNext];
      // while the 'node' is not the last in the chain
      // and the 'node' is not the last in the program.
      while (next != 0 && node < _lenInstruction)
      {
        // if the node we are supposed to point to is in the chain then
        // point to the end of the program instead.
        // Michael McCallum <gholam@xtra.co.nz>
        // FIXME: This is a _hack_ to stop infinite programs.
        // I believe that the implementation of the reluctant matches is wrong but
        // have not worked out a better way yet.
        if (node == pointTo) pointTo = _lenInstruction;
        node += next;
        next = _instruction[node + Regex.offsetNext];
      }

      // if we have reached the end of the program then dont set the pointTo.
      // I'm not sure if this will break any thing but passes all the tests.
      if (node < _lenInstruction)
      {
        // Some patterns result in very large programs which exceed
        // capacity of the short used for specifying signed offset of the
        // next instruction. Example: a{1638}
        int offset = pointTo - node;
        if (offset != (short)offset) throw new RegexpSyntaxException("Exceeded short jump range.");

        // Point the last node in the chain to pointTo.
        _instruction[node + Regex.offsetNext] = (char)(short)offset;
      }
    }

    /// <summary>
    /// Adds a new node
    /// </summary>
    /// <param name="opcode">Opcode for node</param>
    /// <param name="opdata">Opdata for node (only the low 16 bits are currently used)</param>
    /// <returns>Index of new node in program</returns>
    int CreateNode(char opcode, int opdata)
    {
      // Make room for a new node
      Ensure(Regex.nodeSize);

      // Add new node at end
      _instruction[_lenInstruction /* + RE.offsetOpcode */] = opcode;
      _instruction[_lenInstruction + Regex.offsetOpdata] = (char)opdata;
      _instruction[_lenInstruction + Regex.offsetNext] = CharacterClass.DefaultChar;
      _lenInstruction += Regex.nodeSize;

      // Return index of new node
      return _lenInstruction - Regex.nodeSize;
    }

    /// <summary>
    ///Throws a new internal error exception 
    /// </summary>
    void InternalError()
    {
      throw new Exception("Internal error!");
    }

    /// <summary>
    /// Throws a new syntax error exception
    /// </summary>
    /// <param name="s">The Syntax Error</param>
    void SyntaxError(string s)
    {
      throw new RegexpSyntaxException(s);
    }

    /// <summary>
    /// Match bracket {m,n} expression put results in bracket member variables
    /// </summary>
    void Bracket()
    {
      // Current char must be a '{'
      if (_idx >= _len || _pattern[_idx++] != '{')
      {
        InternalError();
      }

      char currentChar = _pattern[_idx];

      // Next char must be a digit
      if (_idx >= _len || !CharacterClass.IsDigit(ref currentChar))
      {
        SyntaxError("Expected digit");
      }

      // Get min ('m' of {m,n}) number
      StringBuilder number = new StringBuilder();
      while (_idx < _len)
      {
        currentChar = _pattern[_idx];
        if (CharacterClass.IsDigit(ref currentChar))
        {
          number.Append(currentChar);
          ++_idx;
        }
        else break;
      }

      try
      {
        _bracketMin = int.Parse(number.ToString());
      }
      catch (Exception)
      {
        SyntaxError("Expected valid number");
      }

      // If out of input, fail
      if (_idx >= _len)
      {
        SyntaxError("Expected comma or right bracket");
      }

      // If end of expr, optional limit is 0
      if (_pattern[_idx] == '}')
      {
        ++_idx;
        _bracketOpt = 0;
        return;
      }

      // Must have at least {m,} and maybe {m,n}.
      if (_idx >= _len || _pattern[_idx++] != ',')
      {
        SyntaxError("Expected comma");
      }

      // If out of input, fail
      if (_idx >= _len)
      {
        SyntaxError("Expected comma or right bracket");
      }

      // If {m,} max is unlimited
      if (_pattern[_idx] == '}')
      {
        ++_idx;
        _bracketOpt = _bracketUnbounded;
        return;
      }

      currentChar = _pattern[_idx];

      // Next char must be a digit
      if (_idx >= _len || !CharacterClass.IsDigit(ref currentChar))
      {
        SyntaxError("Expected digit");
      }

      // Get max number
      number = new StringBuilder();
      while (_idx < _len)
      {
        currentChar = _pattern[_idx];
        if (CharacterClass.IsDigit(ref currentChar))
        {
          number.Append(currentChar);
          ++_idx;
        }
        else break;
      }

      try
      {
        _bracketOpt = int.Parse(number.ToString()) - _bracketMin;
      }
      catch (Exception)
      {
        SyntaxError("Expected valid number");
      }

      // Optional repetitions must be >= 0
      if (_bracketOpt < 0)
      {
        SyntaxError("Bad range");
      }

      // Must have close brace
      if (_idx >= _len || _pattern[_idx++] != '}')
      {
        SyntaxError("Missing close brace");
      }
    }

    /// <summary>
    /// Match an escape sequence.  Handles quoted chars and octal escapes as well
    /// as normal escape characters.  Always advances the input stream by the
    /// right amount. This code "understands" the subtle difference between an
    /// octal escape and a backref.  You can access the type of EscapeCode or
    /// EscapeCode.Complex or EscapeCode.Backrefrence by looking at pattern[idx - 1].
    /// </summary>
    /// <returns>EscapeCode or char if simple escape</returns>
    int Escape()
    {
      // "Shouldn't" happen
      if (_pattern[_idx] != OpCode.Escape)
      {
        InternalError();
      }

      // Escape shouldn't occur as last char in string!
      if (_idx + 1 == _len)
      {
        SyntaxError("Escape terminates string");
      }

      // Switch on char after backslash
      _idx += 2;
      char escapeChar = _pattern[_idx - 1];
      switch (escapeChar)
      {
        case EscapeCode.WordBoundry:
        case EscapeCode.NonWordBoundry:
          return (int)SpecialEscape.Complex;

        case EscapeCode.Alphanumeric:
        case EscapeCode.NonAlphanumeric:
        case EscapeCode.Whitespace:
        case EscapeCode.NonWhitespace:
        case EscapeCode.Digit:
        case EscapeCode.NonDigit:
          return (int)SpecialEscape.CharacterClass;

        case 'u':
        case 'x':
          {
            // Exact required hex digits for escape type
            int hexDigits = (escapeChar == 'u' ? 4 : 2);

            // Parse up to hexDigits characters from input
            int val = 0;
            for (; _idx < _len && hexDigits-- > 0; ++_idx)
            {
              // Get char
              char c = _pattern[_idx];

              // If it's a hexadecimal digit (0-9)
              if (c >= '0' && c <= '9')
              {
                // Compute new value
                val = (val << 4) + c - '0';
              }
              else
              {
                // If it's a hexadecimal letter (a-f)

                CharacterClass.ToLowerAsciiInvariant(ref c);
                if (c >= 'a' && c <= 'f')
                {
                  // Compute new value
                  val = (val << 4) + (c - 'a') + 10;
                }
                else
                {
                  // If it's not a valid digit or hex letter, the escape must be invalid
                  // because hexDigits of input have not been absorbed yet.
                  SyntaxError("Expected " + hexDigits + " hexadecimal digits after \\" + escapeChar);
                }
              }
            }
            return val;
          }

        case 't':
          return CharacterClass.Tab;

        case 'n':
          return CharacterClass.NewLine;

        case 'r':
          return CharacterClass.LineReturn;

        case 'f':
          return '\f';

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          {
            // An octal escape starts with a 0 or has two digits in a row
            if ((_idx < _len && CharacterClass.IsDigit(ref _pattern, ref _idx)) || escapeChar == '0')
            {
              char currentChar = _pattern[_idx];
              // Handle \nnn octal escapes
              int val = escapeChar - '0';
              if (_idx < _len && CharacterClass.IsDigit(ref currentChar))
              {
                currentChar = _pattern[_idx++];
                val = ((val << 3) + (currentChar - '0'));
                if (_idx < _len && CharacterClass.IsDigit(ref currentChar))
                {
                  currentChar = _pattern[_idx++];
                  val = ((val << 3) + (currentChar - '0'));
                }
              }

              return val;
            }

            // It's actually a backreference (\[1-9]), not an escape
            return (int)SpecialEscape.BackReference;
          }
        default:
          {
            // Simple quoting of a character
            return escapeChar;
          }
      }
    }

    /// <summary>
    /// Compile a char class
    /// </summary>
    /// <returns>Index of class node</returns>
    int GetCharacterClass()
    {
      // Check for bad calling or empty class
      if (_pattern[_idx] != '[')
      {
        InternalError();
      }

      // Check for unterminated or empty class
      if ((_idx + 1) >= _len || _pattern[++_idx] == ']')
      {
        SyntaxError("Empty or unterminated class");
      }

      // Check for POSIX character class
      if (_idx < _len && _pattern[_idx] == ':')
      {
        // Skip colon
        ++_idx;

        // POSIX character classes are denoted with lowercase ASCII strings
        int idxStart = _idx;
        while (_idx < _len && _pattern[_idx] >= 'a' && _pattern[_idx] <= 'z') ++_idx;

        // Should be a ":]" to terminate the POSIX character class
        if ((_idx + 1) < _len && _pattern[_idx] == ':' && _pattern[_idx + 1] == ']')
        {
          // Get character class
          String charClass = _pattern.Substring(idxStart, _idx);

          // Select the POSIX class id
          object o = hashPOSIX[(charClass)];
          if (o != null)
          {
            char i = (char)o;
            // Move past colon and right bracket
            _idx += 2;

            // Return new POSIX character class node
            return CreateNode(OpCode.PosixClass, i);
          }
          SyntaxError("Invalid POSIX character class '" + charClass + "'");
        }
        SyntaxError("Invalid POSIX character class syntax");
      }

      // Try to build a class.  Create OP_ANYOF node
      int ret = CreateNode(OpCode.AnyOf, 0);

      // Parse class declaration
      char CHAR_INVALID = char.MaxValue;
      char last = CHAR_INVALID;
      char simpleChar;
      bool include = true;
      bool definingRange = false;
      int idxFirst = _idx;
      char rangeStart = CharacterClass.DefaultChar;
      char rangeEnd;
      CharacterRange range = new CharacterRange();
      while (_idx < _len && _pattern[_idx] != ']')
      {

        // Switch on character
        switch (_pattern[_idx])
        {
          case '^':
            include = !include;
            if (_idx == idxFirst)
            {
              range.Include(char.MinValue, char.MaxValue, true);
            }
            ++_idx;
            continue;

          case OpCode.Escape:
            {
              // Escape always advances the stream
              SpecialEscape c;
              switch (c = (SpecialEscape)Escape())
              {
                case SpecialEscape.Complex:
                case SpecialEscape.BackReference:

                  // Word boundaries and backrefs not allowed in a character class!
                  SyntaxError("Bad character class");
                  goto case SpecialEscape.CharacterClass;
                case SpecialEscape.CharacterClass:

                  // Classes can't be an endpoint of a range
                  if (definingRange)
                  {
                    SyntaxError("Bad character class");
                  }

                  // Handle specific type of class (some are ok)
                  switch (_pattern[_idx - 1])
                  {
                    case EscapeCode.NonWhitespace:
                      range.Include(char.MinValue, 7, include);   // [Min - \b )
                      range.Include((char)11, include);                // ( \n - \f )
                      range.Include(14, 31, include);                   // ( \r - ' ')
                      range.Include(33, char.MaxValue, include);  // (' ' - Max]
                      break;

                    case EscapeCode.NonAlphanumeric:
                      range.Include(char.MinValue, '/', include); // [Min - '0')
                      range.Include(':', '@', include);                 // ('9' - 'A')
                      range.Include('[', '^', include);                 // ('Z' - '_')
                      range.Include('`', include);                      // ('_' - 'a')
                      range.Include('{', char.MaxValue, include); // ('z' - Max]
                      break;

                    case EscapeCode.NonDigit:
                      range.Include(char.MinValue, '/', include); // [Min - '0')
                      range.Include(':', char.MaxValue, include); // ('9' - Max]
                      break;

                    case EscapeCode.Whitespace:
                      range.Include(CharacterClass.Tab, include);
                      range.Include(CharacterClass.LineReturn, include);
                      range.Include('\f', include);
                      range.Include(CharacterClass.NewLine, include);
                      range.Include('\b', include);
                      range.Include(' ', include);
                      break;

                    case EscapeCode.Alphanumeric:
                      range.Include('a', 'z', include);
                      range.Include('A', 'Z', include);
                      range.Include(CharacterClass.Underscore, include);

                      // Fall through!
                      goto case EscapeCode.Digit;
                    case EscapeCode.Digit:
                      range.Include('0', '9', include);
                      break;
                  }

                  // Make last char invalid (can't be a range start)
                  last = CHAR_INVALID;
                  break;

                default:

                  // Escape is simple so treat as a simple char
                  simpleChar = (char)c;
                  break;
              }
              continue;
            }

          case CharacterClass.Hyphen:

            // Start a range if one isn't already started
            if (definingRange)
            {
              SyntaxError("Bad class range");
            }

            definingRange = true;

            // If no last character, start of range is 0
            rangeStart = (char)(last == CHAR_INVALID ? 0 : last);

            // Premature end of range. define up to Character.MAX_VALUE
            if ((_idx + 1) < _len && _pattern[++_idx] == ']')
            {
              simpleChar = char.MaxValue;
              break;
            }
            continue;

          default:
            simpleChar = _pattern[_idx++];
            break;
        }

        // Handle simple character simpleChar
        if (definingRange)
        {
          // if we are defining a range make it now
          rangeEnd = simpleChar;

          // Actually create a range if the range is ok
          if (rangeStart >= rangeEnd)
          {
            SyntaxError("Bad character class");
          }

          range.Include(rangeStart, rangeEnd, include);

          // We are done defining the range
          last = CHAR_INVALID;
          definingRange = false;
        }
        else
        {
          // If simple character and not start of range, include it
          if (_idx >= _len || _pattern[_idx] != CharacterClass.Hyphen)
          {
            range.Include(simpleChar, include);
          }

          last = simpleChar;
        }
      }

      // Shouldn't be out of input
      if (_idx == _len)
      {
        SyntaxError("Unterminated character class");
      }

      // Absorb the ']' end of class marker
      ++_idx;

      // Emit character class definition            
      _instruction[ret + Regex.offsetOpdata] = (char)range.Elements;
      for (int i = 0, e = range.Elements; i < e; ++i)
      {
        Emit((char)range.Minimums[i]);
        Emit((char)range.Maximums[i]);
      }
      return ret;
    }

    /// <summary>
    /// Absorb an atomic char string.  This method is a little tricky because
    /// it can un-include the last char of string if a closure operator follows.
    /// This is correct because *+? have higher precedence than concatentation 
    /// (thus ABC* means AB(C*) and NOT (ABC)*).
    /// </summary>
    /// <returns> Index of new atom node</returns>
    int Atom()
    {
      // Create a string node
      int ret = CreateNode(OpCode.Atom, 0);

      // Length of atom
      int lenAtom = 0;

      // Loop while we've got input
      while (_idx < _len)
      {
        // Is there a next char?
        if ((_idx + 1) < _len)
        {
          char c = _pattern[_idx + 1];

          // If the next 'char' is an escape, look past the whole escape
          if (_pattern[_idx] == OpCode.Escape)
          {
            int idxEscape = _idx;
            Escape();
            if (_idx < _len) c = _pattern[_idx];
            _idx = idxEscape;
          }

          // Switch on next char
          switch (c)
          {
            case '{':
            case '?':
            case '*':
            case OpCode.Plus:

              // If the next char is a closure operator and our atom is non-empty, the
              // current char should bind to the closure operator rather than the atom
              if (lenAtom != 0) goto End;
              else break;
          }
        }

        // Switch on current char
        switch (_pattern[_idx])
        {
          case ']':
          case '^':
          case OpCode.EndOfLine:
          case '.':
          case '[':
          case OpCode.Open:
          case OpCode.Close:
          case '|':
            goto End;

          case '{':
          case '?':
          case '*':
          case OpCode.Plus:
            // We should have an atom by now
            if (lenAtom == 0)
            {
              SyntaxError("Missing operand to closure");
            }

            goto End;

          case OpCode.Escape:
            {
              // Get the escaped char (advances input automatically)
              int idxBeforeEscape = _idx;
              SpecialEscape c = (SpecialEscape)Escape();

              // Check if it's a simple escape (as opposed to, say, a backreference)
              if ((c & SpecialEscape.Mask) == SpecialEscape.Mask)
              {
                // Not a simple escape, so backup to where we were before the escape.
                _idx = idxBeforeEscape;
                //break atomLoop;
                //break;
                goto End;
              }

              // Add escaped char to atom
              Emit((char)c);
              ++lenAtom;
            }
            break;

          default:
            // Add normal char to atom
            Emit(_pattern[_idx++]);
            ++lenAtom;
            break;
        }
      }

    End:
      // This "shouldn't" happen
      if (lenAtom == 0)
      {
        InternalError();
      }

      // Emit the atom Length into the program
      _instruction[ret + Regex.offsetOpdata] = (char)lenAtom;
      return ret;
    }

    /// <summary>
    /// Match a terminal node.
    /// </summary>
    /// <param name="flags">Flags</param>
    /// <returns>Index of terminal node (closeable)</returns>
    int Terminal(int[] flags)
    {
      switch (_pattern[_idx])
      {
        case OpCode.EndOfLine:
        case OpCode.BeginOfLine:
        case OpCode.Any:
          return CreateNode(_pattern[_idx++], 0);
        case '[':
          return GetCharacterClass();
        case OpCode.Open:
          return Expression(flags);
        case OpCode.Close:
          SyntaxError("Unexpected close paren");
          break;
        case '|':
          InternalError();
          break;
        case ']':
          SyntaxError("Mismatched class");
          break;
        case CharacterClass.DefaultChar:
          SyntaxError("Unexpected end of input");
          break;
        case '?':
        case OpCode.Plus:
        case '{':
        case '*':
          SyntaxError("Missing operand to closure");
          break;
        case OpCode.Escape:
          {
            // Don't forget, escape() advances the input stream!
            int idxBeforeEscape = _idx;

            // Switch on escaped character
            switch ((SpecialEscape)Escape())
            {
              case SpecialEscape.CharacterClass:
              case SpecialEscape.Complex:
                {
                  flags[0] &= ~NODE_NULLABLE;
                  return CreateNode(OpCode.Escape, _pattern[_idx - 1]);
                }
              case SpecialEscape.BackReference:
                {
                  char backreference = (char)(_pattern[_idx - 1] - '0');
                  if (_parens <= backreference)
                  {
                    SyntaxError("Bad backreference");
                  }
                  flags[0] |= NODE_NULLABLE;
                  return CreateNode(OpCode.BackRef, backreference);
                }

              default:
                {
                  // We had a simple escape and we want to have it end up in
                  // an atom, so we back up and fall though to the default handling
                  _idx = idxBeforeEscape;
                  flags[0] &= ~NODE_NULLABLE;
                  break;
                }
            }
            break;
          }
      }

      // Everything above either fails or returns.
      // If it wasn't one of the above, it must be the start of an atom.
      flags[0] &= ~NODE_NULLABLE;
      return Atom();
    }

    /// <summary>
    /// Compile a possibly closured terminal
    /// </summary>
    /// <param name="flags">Flags passed by reference</param>
    /// <returns>Index of closured node</returns>
    int Closure(int[] flags)
    {
      // Before terminal
      int idxBeforeTerminal = _idx;

      // Values to pass by reference to terminal()
      int[] terminalFlags = { NODE_NORMAL };

      // Get terminal symbol
      int ret = Terminal(terminalFlags);

      // Or in flags from terminal symbol
      flags[0] |= terminalFlags[0];

      // Advance input, set NODE_NULLABLE flag and do sanity checks
      if (_idx >= _len)
      {
        return ret;
      }

      bool greedy = true;
      char closureType = _pattern[_idx];
      switch (closureType)
      {
        case '?':
        case '*':
          {
            // The current node can be null
            flags[0] |= NODE_NULLABLE;
            //break;
            goto case OpCode.Plus;
            // Drop through
          }
        case OpCode.Plus:
          {
            // Eat closure character
            ++_idx;
            //break;
            goto case '{';
            // Drop through
          }
        case '{':
          {
            // Don't allow blantant stupidity
            int opcode = _instruction[ret /* + RE.offsetOpcode */];
            if (opcode == OpCode.BeginOfLine || opcode == OpCode.EndOfLine) SyntaxError("Bad closure operand");
            if ((terminalFlags[0] & NODE_NULLABLE) != 0) SyntaxError("Closure operand can't be nullable");
            break;
          }
      }

      // If the next char is a '?', make the closure non-greedy (reluctant)
      if (_idx < _len && _pattern[_idx] == '?')
      {
        ++_idx;
        greedy = false;
      }

      if (greedy)
      {
        // Actually do the closure now
        switch (closureType)
        {
          case '{':
            {
              Bracket();
              int bracketEnd = _idx;
              int bracketMin = this._bracketMin;
              int bracketOpt = this._bracketOpt;

              // Pointer to the last terminal
              int pos = ret;

              // Process min first
              for (int c = 0; c < bracketMin; c++)
              {
                // Rewind stream and run it through again - more matchers coming
                _idx = idxBeforeTerminal;
                SetNextOfEnd(pos, pos = Terminal(terminalFlags));
              }

              // Do the right thing for maximum ({m,})
              if (bracketOpt == _bracketUnbounded)
              {
                // Drop through now and closure expression.
                // We are done with the {m,} expr, so skip rest
                _idx = bracketEnd;
                InsertNode(OpCode.Star, 0, pos);
                SetNextOfEnd(pos + Regex.nodeSize, pos);
                break;
              }
              else if (bracketOpt > 0)
              {
                int[] opt = new int[bracketOpt + 1];
                // Surround first optional terminal with MAYBE
                InsertNode(OpCode.Maybe, 0, pos);
                opt[0] = pos;

                // Add all the rest optional terminals with preceeding MAYBEs
                for (int c = 1; c < bracketOpt; ++c)
                {
                  opt[c] = CreateNode(OpCode.Maybe, 0);
                  // Rewind stream and run it through again - more matchers coming
                  _idx = idxBeforeTerminal;
                  Terminal(terminalFlags);
                }

                // Tie ends together
                int end = opt[bracketOpt] = CreateNode(OpCode.Nothing, 0);
                for (int c = 0; c < bracketOpt; ++c)
                {
                  SetNextOfEnd(opt[c], end);
                  SetNextOfEnd(opt[c] + Regex.nodeSize, opt[c + 1]);
                }
              }
              else
              {
                // Rollback terminal - no opt matchers present
                _lenInstruction = pos;
                CreateNode(OpCode.Nothing, 0);
              }

              // We are done. skip the reminder of {m,n} expr
              _idx = bracketEnd;
              break;
            }

          case '?':
            {
              InsertNode(OpCode.Maybe, 0, ret);
              int n = CreateNode(OpCode.Nothing, 0);
              SetNextOfEnd(ret, n);
              SetNextOfEnd(ret + Regex.nodeSize, n);
              break;
            }

          case '*':
            {
              InsertNode(OpCode.Star, 0, ret);
              SetNextOfEnd(ret + Regex.nodeSize, ret);
              break;
            }

          case OpCode.Plus:
            {
              InsertNode(OpCode.Continue, 0, ret);
              int n = CreateNode(OpCode.Plus, 0);
              SetNextOfEnd(ret + Regex.nodeSize, n);
              SetNextOfEnd(n, ret);
              break;
            }
        }
      }
      else
      {
        // Actually do the closure now
        switch (closureType)
        {
          case '?':
            {
              InsertNode(OpCode.ReluctantMaybe, 0, ret);
              int n = CreateNode(OpCode.Nothing, 0);
              SetNextOfEnd(ret, n);
              SetNextOfEnd(ret + Regex.nodeSize, n);
              break;
            }

          case '*':
            {
              InsertNode(OpCode.ReluctantStar, 0, ret);
              SetNextOfEnd(ret + Regex.nodeSize, ret);
              break;
            }

          case OpCode.Plus:
            {
              InsertNode(OpCode.Continue, 0, ret);
              int n = CreateNode(OpCode.ReluctantPlus, 0);
              SetNextOfEnd(n, ret);
              SetNextOfEnd(ret + Regex.nodeSize, n);
              break;
            }
        }
      }

      return ret;
    }

    /// <summary>
    /// Compile body of one branch of an or operator (implements concatenation)
    /// </summary>
    /// <param name="flags">Flags</param>
    /// <returns>Pointer to first node in the branch</returns>
    int Branch(int[] flags)
    {
      // Get each possibly closured piece and concat
      int anode;
      int ret = -1;
      int chain = -1;
      int[] closureFlags = new int[1];
      bool nullable = true;
      while (_idx < _len && _pattern[_idx] != '|' && _pattern[_idx] != OpCode.Close)
      {
        // Get new node
        closureFlags[0] = NODE_NORMAL;
        anode = Closure(closureFlags);
        if (closureFlags[0] == NODE_NORMAL)
        {
          nullable = false;
        }

        // If there's a chain, Append to the end
        if (chain != -1)
        {
          SetNextOfEnd(chain, anode);
        }

        // Chain starts at current
        chain = anode;
        if (ret == -1)
        {
          ret = anode;
        }
      }

      // If we don't run loop, make a nothing node
      if (ret == -1)
      {
        ret = CreateNode(OpCode.Nothing, 0);
      }

      // Set nullable flag for this branch
      if (nullable)
      {
        flags[0] |= NODE_NULLABLE;
      }

      return ret;
    }

    /// <summary>
    /// Compile an expression with possible parens around it.  Paren matching
    ///  is done at this level so we can tie the branch tails together.
    /// </summary>
    /// <param name="flags">Flags</param>
    /// <returns>Node index of expression in instruction array</returns>
    int Expression(int[] flags)
    {
      // Create open paren node unless we were called from the top level (which has no parens)
      int paren = -1;
      int ret = -1;
      int closeParens = _parens;
      if ((flags[0] & NODE_TOPLEVEL) == 0 && _pattern[_idx] == OpCode.Open)
      {
        // if its a cluster ( rather than a proper subexpression ie with backrefs )
        if (_idx + 2 < _len && _pattern[_idx + 1] == '?' && _pattern[_idx + 2] == ':')
        {
          paren = 2;
          _idx += 3;
          ret = CreateNode(OpCode.OpenCluster, 0);
        }
        else
        {
          paren = 1;
          ++_idx;
          ret = CreateNode(OpCode.Open, _parens++);
        }
      }
      flags[0] &= ~NODE_TOPLEVEL;

      // Process contents of first branch node
      bool open = false;
      int abranch = Branch(flags);
      if (ret == -1)
      {
        ret = abranch;
      }
      else
      {
        SetNextOfEnd(ret, abranch);
      }

      // Loop through branches
      while (_idx < _len && _pattern[_idx] == '|')
      {
        // Now open the first branch since there are more than one
        if (!open)
        {
          InsertNode(OpCode.Branch, 0, abranch);
          open = true;
        }

        ++_idx;
        SetNextOfEnd(abranch, abranch = CreateNode(OpCode.Branch, 0));
        Branch(flags);
      }

      // Create an ending node (either a close paren or an OP_END)
      int end;
      if (paren > 0)
      {
        if (_idx < _len && _pattern[_idx] == OpCode.Close)
        {
          ++_idx;
        }
        else
        {
          SyntaxError("Missing close paren");
        }

        if (paren == 1)
        {
          end = CreateNode(OpCode.Close, closeParens);
        }
        else
        {
          end = CreateNode(OpCode.CloseCluster, 0);
        }
      }
      else
      {
        end = CreateNode(OpCode.EndProgram, 0);
      }

      // Append the ending node to the ret nodelist
      SetNextOfEnd(ret, end);

      // Hook the ends of each branch to the end node
      int currentNode = ret;
      int nextNodeOffset = _instruction[currentNode + Regex.offsetNext];
      // while the next node o
      while (nextNodeOffset != 0 && currentNode < _lenInstruction)
      {
        // If branch, make the end of the branch's operand chain point to the end node.
        if (_instruction[currentNode /* + RE.offsetOpcode */] == OpCode.Branch)
        {
          SetNextOfEnd(currentNode + Regex.nodeSize, end);
        }

        nextNodeOffset = _instruction[currentNode + Regex.offsetNext];
        currentNode += nextNodeOffset;
      }

      // Return the node list
      return ret;
    }

    /// <summary>
    /// Compiles a regular expression pattern into a program runnable by the pattern matcher class 'RE'.
    /// </summary>
    /// <param name="pattern">Regular expression pattern to compile</param>
    /// <returns></returns>
    internal RegexProgram Compile(String pattern)
    {
      //Check if the pattersn is already compiled
      if (Regex.CacheSize > 0 && Regex.Cache.Contains(pattern))
      {
        foreach (RegexProgram regexProgram in Regex.Cache.ToArray())
          if (regexProgram.Pattern.Equals(pattern)) return regexProgram;
        throw new Exception("Cache Error RegexCompiler.cs Compile Method");
      }
      else
      {
        //Maintain CacheSize Requirement
        if (Regex.CacheSize > 0 && Regex.Cache.Count >= Regex.CacheSize)
        {
          Regex.Cache.Pop();
        }

        // Initialize variables for compilation
        _pattern = pattern;     // Save pattern in instance variable
        _len = pattern.Length;  // Precompute pattern Length for speed
        _idx = 0;               // Set parsing index to the first character
        _lenInstruction = 0;    // Set emitted instruction count to zero
        _parens = 1;            // Set paren level to 1 (the implicit outer parens)

        // Initialize pass by reference flags value
        int[] flags = { NODE_TOPLEVEL };

        // Parse expression
        Expression(flags);

        // Should be at end of input
        if (_idx != _len)
        {
          if (pattern[_idx] == OpCode.Close) SyntaxError("Unmatched close paren");
          SyntaxError("Unexpected input remains");
        }

        //Compile the result

        char[] ins = new char[_lenInstruction];
        Array.Copy(_instruction, 0, ins, 0, _lenInstruction);
        //ins.CopyTo(instruction, 0);

        RegexProgram result = new RegexProgram(_parens, ins, pattern);

        //If caching is enabled add the pattern to the cache
        //Should probably check for the Compiled Flag before putting it in
        if (Regex.CacheSize > 0 && !Regex.Cache.Contains(pattern))
        {
          Regex.Cache.Push(result);
        }

        return result;
      }
    }

  }
}
