NAME=System.Text

CSC=csc.exe
MDP=../assemblies/CanFly.Build.exe
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

all: $(OUTPATH)/$(NAME).pe

$(OUTPATH)/$(NAME).pe : $(SRC)
	$(CSC) $(FLAGS) $(LIBS) $(SRC) -out:$(OUTPATH)/$(NAME).dll
	$(MDP) -c $(OUTPATH)/$(NAME).dll -p $(OUTPATH)/$(NAME).pe

install:
	cp $(NAME).pe $(OUTPATH)

clean:

fclean: clean
	$(RM) $(NAME)

re: fclean all

