NAME=diy-efis

CC=csc.exe
FLAGS=
LIBS=
RM =rm -rf


ASSEMBLIES := \
	./assemblies/mscorlib.pe \
	./assemblies/System.Collections.pe \
	./assemblies/System.Math.pe \
	./assemblies/System.Text.pe \
	./assemblies/CanFly.Neutron.pe \
	./assemblies/CanFly.Photon.pe \
	./assemblies/CanFly.Proton.pe \
	./assemblies/CanFly.DiyEfis.pe

SUBDIRS := \
	CoreLibrary \
	System.Collections \
	System.Math \
	System.Text \
	CanFly.Photon \
	CanFly.Proton \
	CanFly.DiyEfis

define SUBPROJ
	$(MAKE) -C $(1) $(2)
endef

.PHONY: all
all:
	$(call SUBPROJ, CoreLibrary, all)
	$(call SUBPROJ, System.Collections, all)
	$(call SUBPROJ, System.Text, all)
	$(call SUBPROJ, CanFly.Photon, all)
	$(call SUBPROJ, CanFly.Proton, all)
	$(call SUBPROJ, CanFly.DiyEfis, all)
	rm -f emulator/fs/diy-efis.zip
	rm -f emulator/fs/diy-efis.app
	PowerShell Compress-Archive assemblies/*.pe emulator/fs/diy-efis.zip
	mv emulator/fs/diy-efis.zip emulator/fs/diy-efis.app

.PHONY: clean
clean:
	$(call SUBPROJ, CoreLibrary, clean)
	$(call SUBPROJ, System.Collections, clean)
	$(call SUBPROJ, System.Math, clean)
	$(call SUBPROJ, System.Text, clean)
	$(call SUBPROJ, CanFly.Neutron, clean)
	$(call SUBPROJ, CanFly.Photon, clean)
	$(call SUBPROJ, CanFly.Proton, clean)
	$(call SUBPROJ, CanFly.DiyEfis, clean)
