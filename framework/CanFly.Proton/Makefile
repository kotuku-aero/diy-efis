NAME=CanFly.Proton

CSC=csc.exe
MDP=../../tools/CanFly.Build.exe
FLAGS=-langversion:1 -nostdlib -target:library -debug:full
MDPFLAGS=

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
	Pens.cs \
	Properties/AssemblyInfo.cs

LIBS=-reference:../assemblies/mscorlib.dll;../assemblies/CanFly.Photon.dll;../assemblies/System.Collections.dll
RM =rm -rf

OUTPATH=../assemblies

all: $(OUTPATH)/$(NAME).pe

debug: MDPFLAGS += -d
debug: $(OUTPATH)/$(NAME).pe

release: $(OUTPATH)/$(NAME).pe
release: FLAGS += -optimize

$(OUTPATH)/$(NAME).pe : $(SRC)
	$(CSC) $(FLAGS) $(LIBS) $(SRC) -out:$(OUTPATH)/$(NAME).dll
	$(MDP) -c $(OUTPATH)/$(NAME).dll -p $(OUTPATH)/$(NAME).pe  $(MDPFLAGS)

clean:

fclean: clean
	$(RM) $(NAME)

re: fclean all

