NAME=Kotuku

define SUBPROJ
	$(MAKE) -C $(1) $(2)
endef

LIBRARIES= \
	assemblies/mscorlib.pe \
	assemblies/System.Collections.pe \
	assemblies/System.Text.pe \
	assemblies/CanFly.Photon.pe \
	assemblies/CanFly.Proton.pe

LIBRARIES_COMPRESS= \
	assemblies/mscorlib.pe, \
	assemblies/System.Collections.pe, \
	assemblies/System.Text.pe, \
	assemblies/CanFly.Photon.pe, \
	assemblies/CanFly.Proton.pe



.PHONY: debug
debug: efi-debug pfd-debug

release: efi-release pfd-release

assemblies/mscorlib.pe:
	$(call SUBPROJ, CoreLibrary, debug)

assemblies/System.Collections.pe:
	$(call SUBPROJ, System.Collections, debug)

assemblies/System.Text.pe:
	$(call SUBPROJ, System.Text, debug)

assemblies/CanFly.Photon.pe:
	$(call SUBPROJ, CanFly.Photon, debug)

assemblies/CanFly.Proton.pe:
	$(call SUBPROJ, CanFly.Proton, debug)

libs-debug: $(LIBRARIES)

libs-release: $(LIBRARIES)
	$(call SUBPROJ, CoreLibrary, release)
	$(call SUBPROJ, System.Collections, release)
	$(call SUBPROJ, System.Text, release)
	$(call SUBPROJ, CanFly.Photon, release)
	$(call SUBPROJ, CanFly.Proton, release)

efi-debug: libs-debug
	$(call SUBPROJ, Kotuku.Efi, debug)
	rm -f emulator/fs/kotuku-efi.zip
	rm -f emulator/fs/kotuku-efi.app
	PowerShell Compress-Archive -Path $(LIBRARIES_COMPRESS), assemblies/Kotuku.Efi.pe -DestinationPath emulator/fs/kotuku-efi.zip
	mv emulator/fs/kotuku-efi.zip emulator/fs/kotuku-efi.app

efi-release: libs-release
	$(call SUBPROJ, Kotuku.Efi, release)
	rm -f emulator/fs/kotuku-efi.zip
	rm -f emulator/fs/kotuku-efi.app
	PowerShell Compress-Archive -Path $(LIBRARIES_COMPRESS), assemblies/Kotuku.Efi.pe -DestinationPath emulator/fs/kotuku-efi.zip
	mv emulator/fs/kotuku-efi.zip emulator/fs/kotuku-efi.app

pfd-debug: libs-debug
	$(call SUBPROJ, Kotuku.Pfd, debug)
	rm -f emulator/fs/kotuku-pfd.zip
	rm -f emulator/fs/kotuku-pfd.app
	PowerShell Compress-Archive -Path $(LIBRARIES_COMPRESS), assemblies/Kotuku.Pfd.pe -DestinationPath emulator/fs/kotuku-pfd.zip
	mv emulator/fs/kotuku-pfd.zip emulator/fs/kotuku-pfd.app

pfd-release: libs-release
	$(call SUBPROJ, Kotuku.Pfd, release)
	rm -f emulator/fs/kotuku-pfd.zip
	rm -f emulator/fs/kotuku-pfd.app
	PowerShell Compress-Archive -Path $(LIBRARIES_COMPRESS), assemblies/Kotuku.Pfd.pe -DestinationPath emulator/fs/kotuku-pfd.zip
	mv emulator/fs/kotuku-pfd.zip emulator/fs/kotuku-pfd.app
	