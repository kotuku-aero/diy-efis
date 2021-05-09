NAME=System.Text

CSC=csc.exe
MDP=../../tools/CanFly.Build.exe
FLAGS=-langversion:1 -nostdlib -noconfig -target:library -debug:full -nowarn:0659;3001;0661;3019;3002;3003;3005;0436
MDPFLAGS=

SRC= \
	Decoder.cs \
	Encoding.cs \
	StringBuilder.cs \
	UTF8Decoder.cs \
	UTF8Encoding.cs \
	Properties/AssemblyInfo.cs \
	RegularExpressions\Capture.cs \
	RegularExpressions\CaptureCollection.cs \
	RegularExpressions\CaptureEnumerator.cs \
	RegularExpressions\CharacterClass.cs \
	RegularExpressions\CharacterRange.cs \
	RegularExpressions\EscapeCode.cs \
	RegularExpressions\Group.cs \
	RegularExpressions\GroupCollection.cs \
	RegularExpressions\GroupEnumerator.cs \
	RegularExpressions\ICharacterIterator.cs \
	RegularExpressions\Match.cs \
	RegularExpressions\MatchCollection.cs \
	RegularExpressions\MatchEnumerator.cs \
	RegularExpressions\MatchEvaluator.cs \
	RegularExpressions\OpCode.cs \
	RegularExpressions\POSIXCharacterClass.cs \
	RegularExpressions\ProgramOptions.cs \
	RegularExpressions\Regex.cs \
	RegularExpressions\RegexCompiler.cs \
	RegularExpressions\RegexExecutionTimeException.cs \
	RegularExpressions\RegexOptions.cs \
	RegularExpressions\RegexProgram.cs \
	RegularExpressions\ReplaceOptions.cs \
	RegularExpressions\SpecialEscape.cs \
	RegularExpressions\StringCharacterIterator.cs \
	RegularExpressions\SyntaxException.cs

LIBS=-reference:../assemblies/mscorlib.dll;../assemblies/System.Collections.dll
RM =rm -rf

OUTPATH=../assemblies

all: $(OUTPATH)/$(NAME).pe

debug: MDPFLAGS += -d
debug: $(OUTPATH)/$(NAME).pe

release: $(OUTPATH)/$(NAME).pe
release: FLAGS += -optimize

$(OUTPATH)/$(NAME).pe : $(SRC)
	$(CSC) $(FLAGS) $(LIBS) $(SRC) -out:$(OUTPATH)/$(NAME).dll
	$(MDP) -c $(OUTPATH)/$(NAME).dll -p $(OUTPATH)/$(NAME).pe $(MDPFLAGS)

install:
	cp $(NAME).pe $(OUTPATH)

clean:

fclean: clean
	$(RM) $(NAME)

re: fclean all

