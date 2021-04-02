NAME=System.Math

CSC=csc.exe
MDP=CanFly.Build.exe
FLAGS=-langversion:1
MDPFLAGS=-d $(NAME).pdb

SRC= \
	Math.cs \
	Properties/AssemblyInfo.cs

LIBS=-reference:mscorlib
RM =rm -rf

OUTPATH=Debug

all: $(NAME)

$(NAME) : $(SRC)
	$(CSC) $(FLAGS) $(LIBS) $(SRC) -out:$(OUTPATH)/$(NAME).dll
	$(MDP) -c $(OUTPATH)/$(NAME).dll -o $(OUTPATH)/$(NAME).pe

clean:

fclean: clean
	$(RM) $(NAME)

re: fclean all

