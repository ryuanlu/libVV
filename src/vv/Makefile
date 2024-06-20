include ../../build.mk

TARGET:=vv

XDG_SHELL_PROTOCOL:=xdg-shell
XDG_SHELL_PROTOCOL_SOURCE:=$(XDG_SHELL_PROTOCOL)-protocol.c
XDG_SHELL_PROTOCOL_HEADER:=$(XDG_SHELL_PROTOCOL)-client-protocol.h
XDG_SHELL_PROTOCOL_XML:=/usr/share/wayland-protocols/stable/xdg-shell/$(XDG_SHELL_PROTOCOL).xml


$(XDG_SHELL_PROTOCOL_SOURCE): $(XDG_SHELL_PROTOCOL_XML)
	@printf "\tGEN\t$@\n"
	wayland-scanner private-code $< $@
$(XDG_SHELL_PROTOCOL_HEADER): $(XDG_SHELL_PROTOCOL_XML)
	@printf "\tGEN\t$@\n"
	wayland-scanner client-header $< $@


PKG_CONFIG_PKGS:=egl wayland-client wayland-egl gbm glesv2

$(TARGET)_src_files:=bmp.c colormap.c gbm.c isosurface_extraction.c main.c panel.c tty.c viewer.c volume_rendering.c wayland.c $(XDG_SHELL_PROTOCOL_SOURCE)
$(TARGET)_resource_files:=help.txt
$(TARGET)_cflags:=$(COMMON_CFLAGS) $(DEBUG_FLAGS) $(shell pkgconf --cflags $(PKG_CONFIG_PKGS)) -I../libVV
$(TARGET)_ldflags:=$(DEBUG_FLAGS) -lm $(shell pkgconf --libs $(PKG_CONFIG_PKGS)) -L ../libVV/ -lVV

$(eval $(call define_c_targets,$(TARGET)))

wayland.o: | $(XDG_SHELL_PROTOCOL_HEADER)

all: $(TARGET) $(TARGET_LIB)

clean: $(TARGET)_clean
	@printf "\tCLEAN\n"
	rm -f $(XDG_SHELL_PROTOCOL_SOURCE) $(XDG_SHELL_PROTOCOL_HEADER)
