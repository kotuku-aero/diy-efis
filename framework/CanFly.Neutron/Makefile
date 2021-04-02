NAME=CanFly.Neutron

CSC=csc.exe
MDP=CanFly.Build.exe
FLAGS=-langversion:1 -nostdlib -target:library
MDPFLAGS=-d $(NAME).pdb

SRC= \
	CanFlyMsg.cs \
	DirectoryEnumerator.cs \
	EventPRoxy.cs \
	Stream.cs \
	Thread.cs \
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

