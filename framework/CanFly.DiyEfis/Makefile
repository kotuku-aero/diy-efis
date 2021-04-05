NAME=Canfly.DiyEfis

CSC=csc.exe
MDP=../assemblies/CanFly.Build.exe
FLAGS=-langversion:1 -nostdlib -target:exe
MDPFLAGS=-d $(NAME).pdb

SRC= \
	Program.cs \
	Properties/AssemblyInfo.cs

LIBS=-reference:../assemblies/mscorlib.dll;../assemblies/CanFly.Photon.dll;../assemblies/CanFly.Proton.dll
RM =rm -rf

OUTPATH=../assemblies

all: $(OUTPATH)/$(NAME).pe

$(OUTPATH)/$(NAME).pe : $(SRC)
	$(CSC) $(FLAGS) $(LIBS) $(SRC) -out:$(OUTPATH)/$(NAME).exe
	$(MDP) -c $(OUTPATH)/$(NAME).exe -p $(OUTPATH)/$(NAME).pe

clean:

fclean: clean
	$(RM) $(NAME)

re: fclean all

