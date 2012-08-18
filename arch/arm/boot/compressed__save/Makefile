#
# linux/arch/arm/boot/compressed/Makefile
#
# create a compressed vmlinuz image from the original vmlinux
#
HEAD	= head.o
OBJS	= misc.o
FONTC	= $(srctree)/drivers/video/console/font_acorn_8x8.c
#
# Architecture dependencies
#

OBJS		+= head-omap.o

#
# We now have a PIC decompressor implementation.  Decompressors running
# from RAM should not define ZTEXTADDR.  Decompressors running directly
# from ROM or Flash must define ZTEXTADDR (preferably via the config)
# FIXME: Previous assignment to ztextaddr-y is lost here. See SHARK

ZTEXTADDR	:= 0
ZBSSADDR	:= ALIGN(8)


SEDFLAGS	= s/TEXT_START/$(ZTEXTADDR)/;s/BSS_START/$(ZBSSADDR)/

suffix_$ := gzip

targets       := vmlinux vmlinux.lds piggy.gz piggy.o font.o font.c \
		 head.o misc.o $(OBJS)


ifeq ($(CONFIG_FUNCTION_TRACER),y)
ORIG_CFLAGS := $(KBUILD_CFLAGS)
KBUILD_CFLAGS = $(subst -pg, , $(ORIG_CFLAGS))
endif

ccflags-y := -fpic -fno-builtin
asflags-y := -Wa,-march=all

# Supply ZRELADDR to the decompressor via a linker symbol.
ifneq ($(CONFIG_AUTO_ZRELADDR),y)
LDFLAGS_vmlinux += --defsym zreladdr=$(ZRELADDR)
endif
ifeq ($(CONFIG_CPU_ENDIAN_BE8),y)
LDFLAGS_vmlinux += --be8
endif
# ?
LDFLAGS_vmlinux += -p
# Report unresolved symbol references
LDFLAGS_vmlinux += --no-undefined
# Delete all temporary local symbols
LDFLAGS_vmlinux += -X
# Next argument is a linker script
LDFLAGS_vmlinux += -T

# For __aeabi_uidivmod
AFLAGS_lib1funcs.o +=-Wa,-march=armv7-a$(plus_sec)
lib1funcs = $(obj)/lib1funcs.o

$(obj)/lib1funcs.S: $(srctree)/arch/$(SRCARCH)/lib/lib1funcs.S FORCE
	$(call cmd,shipped)

# For __aeabi_llsl
AFLAGS_ashldi3.o +=-Wa,-march=armv7-a$(plus_sec)
ashldi3 = $(obj)/ashldi3.o

$(obj)/ashldi3.S: $(srctree)/arch/$(SRCARCH)/lib/ashldi3.S FORCE
	$(call cmd,shipped)

# We need to prevent any GOTOFF relocs being used with references
# to symbols in the .bss section since we cannot relocate them
# independently from the rest at run time.  This can be achieved by
# ensuring that no private .bss symbols exist, as global symbols
# always have a GOT entry which is what we need.
# The .data section is already discarded by the linker script so no need
# to bother about it here.

$(obj)/piggy.o: $(obj)/../Image FORCE
	$(call if_changed,gzip)

AFLAGS_piggy.gzip.o += -Wa,-march=armv7-a$(plus_sec)
$(obj)/piggy.gzip.o:  $(obj)/piggy.gzip FORCE


$(obj)/vmlinux: $(obj)/vmlinux.lds $(obj)/$(HEAD) $(obj)/piggy.gzip.o \
	 	$(addprefix $(obj)/, $(OBJS)) $(lib1funcs) FORCE
	$(call if_changed,ld)
	@:


$(obj)/vmlinux.lds: $(obj)/vmlinux.lds.in arch/arm/boot/Makefile $(KCONFIG_CONFIG)
	@sed "$(SEDFLAGS)" < $< > $@
