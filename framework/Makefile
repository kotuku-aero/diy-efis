NAME=diy-efis

ASSEMBLIES := \
	./assemblies/mscorlib.pe \
	./assemblies/System.Collections.pe \
	./assemblies/System.Text.pe \
	./assemblies/CanFly.Neutron.pe \
	./assemblies/CanFly.Photon.pe \
	./assemblies/CanFly.Proton.pe \
	./assemblies/CanFly.DiyEfis.pe

SUBDIRS := \
	CoreLibrary \
	System.Collections \
	System.Text \
	CanFly.Photon \
	CanFly.Proton \
	CanFly.DiyEfis

define SUBPROJ
	$(MAKE) -C $(1) $(2)
endef

.PHONY: debug
debug:
	$(call SUBPROJ, CoreLibrary, debug)
	$(call SUBPROJ, System.Collections, debug)
	$(call SUBPROJ, System.Text, debug)
	$(call SUBPROJ, CanFly.Photon, debug)
	$(call SUBPROJ, CanFly.Proton, debug)
	$(call SUBPROJ, CanFly.DiyEfis, debug)
	rm -f emulator/fs/diy-efis.zip
	rm -f emulator/fs/diy-efis.app
	PowerShell Compress-Archive assemblies/*.pe emulator/fs/diy-efis.zip
	mv emulator/fs/diy-efis.zip emulator/fs/diy-efis.app

.PHONY: release
release:
	$(call SUBPROJ, CoreLibrary, release)
	$(call SUBPROJ, System.Collections, release)
	$(call SUBPROJ, System.Text, release)
	$(call SUBPROJ, CanFly.Photon, release)
	$(call SUBPROJ, CanFly.Proton, release)
	$(call SUBPROJ, CanFly.DiyEfis, release)
	rm -f emulator/fs/diy-efis.zip
	rm -f emulator/fs/diy-efis.app
	PowerShell Compress-Archive assemblies/*.pe emulator/fs/diy-efis.zip
	mv emulator/fs/diy-efis.zip emulator/fs/diy-efis.app

.PHONY: clean
clean:
	$(call SUBPROJ, CoreLibrary, clean)
	$(call SUBPROJ, System.Collections, clean)
	$(call SUBPROJ, System.Text, clean)
	$(call SUBPROJ, CanFly.Photon, clean)
	$(call SUBPROJ, CanFly.Proton, clean)
	$(call SUBPROJ, CanFly.DiyEfis, clean)
	rm -f emulator/fs/diy-efis.zip
	rm -f emulator/fs/diy-efis.app
