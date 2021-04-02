NAME=System.Text

CSC=csc.exe
MDP=CanFly.Build.exe
FLAGS=-langversion:1 -nostdlib -target:library
MDPFLAGS=-d $(NAME).pdb

SRC= \
	Decoder.cs \
	Encoding.cs \
	StringBuilder.cs \
	UTF8Decoder.cs \
	UTF8Encoding.cs \
	Properties/AssemblyInfo.cs

LIBS=-reference:../assemblies/mscorlib.dll
RM =rm -rf

OUTPATH=../assemblies

all: $(NAME)

$(NAME) : $(SRC)
	$(CSC) $(FLAGS) $(LIBS) $(SRC) -out:$(OUTPATH)/$(NAME).dll
#	$(MDP) -c $(OUTPATH)/$(NAME).dll -o $(OUTPATH)/$(NAME).pe

clean:

fclean: clean
	$(RM) $(NAME)

re: fclean all

