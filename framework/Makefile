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
	CanFly.Neutron \
	CanFly.Photon \
	CanFly.Proton \
	CanFly.DiyEfis

define FOREACH
	for DIR in SUBDIRS; do \
		$(MAKE) -C $$FIR $(1); \
	done
endef

.PHONY: build
build:
	$(call FOREACH, build)

.PHONY: clean
clean:
	$(call FOREACH, clean)
