NAME=CanFly.Proton

CSC=csc.exe
MDP=CanFly.Build.exe
FLAGS=-langversion:1 -nostdlib -target:library
MDPFLAGS=-d $(NAME).pdb

SRC= \
	AirspeedWidget.cs \
	AltitudeWidget.cs \
	AnnunciatorWidget.cs \
	AttitudeWidget.cs \
	GaugeWidget.cs \
	GpsWidget.cs \
	HSIWidget.cs \
	Keys.cs \
	LayoutWidget.cs \
	Menu.cs \
	MenuItem.cs \
	MenuItemCancel.cs \
	MenuItemChecklist.cs \
	MenuItemEdit.cs \
	MenuItemEnter.cs \
	MenuItemEvent.cs \
	MenuItemItem.cs \
	MenuItemMenu.cs \
	MenuItemPopup.cs \
	Properties/AssemblyInfo.cs

LIBS=-reference:../assemblies/mscorlib.dll;../assemblies/CanFly.Neutron.dll;../assemblies/CanFly.Photon.dll
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

