NAME=CanFly.Photon

CSC=csc.exe
MDP=../assemblies/CanFly.Build.exe
FLAGS=-langversion:1 -nostdlib -target:library
MDPFLAGS=-d $(NAME).pdb

SRC= \
	Canvas.cs \
	Color.cs \
	Extent.cs \
	Font.cs \
	GDIObject.cs \
	Pen.cs \
	Photon.cs \
	Point.cs \
	PointArray.cs \
	Rect.cs \
	Screen.cs \
	Widget.cs \
	Properties/AssemblyInfo.cs

LIBS=-reference:../assemblies/mscorlib.dll;../assemblies/CanFly.Neutron.dll;../assemblies/System.Math.dll
RM =rm -rf

OUTPATH=../assemblies

all: $(NAME)

$(NAME) : $(SRC)
	$(CSC) $(FLAGS) $(LIBS) $(SRC) -out:$(OUTPATH)/$(NAME).dll
	$(MDP) -c $(OUTPATH)/$(NAME).dll -p $(OUTPATH)/$(NAME).pe

clean:

fclean: clean
	$(RM) $(NAME)

re: fclean all

