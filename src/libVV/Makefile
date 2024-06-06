include ../../build.mk

TARGET:=libVV.so

PKG_CONFIG_PKGS:=glesv2 egl

$(TARGET)_src_files:=gles.c isosurface.c isosurface_renderer.c matrix.c raw.c renderer.c vertex_buffer.c volume.c volume_renderer.c volume_texture.c
$(TARGET)_resource_files:=volume.vert volume.frag isosurface.vert isosurface.frag
$(TARGET)_cflags:=-Wall $(shell pkgconf --cflags $(PKG_CONFIG_PKGS)) $(DEBUG_FLAGS) -fPIC
$(TARGET)_ldflags:=-Wall -lm $(shell pkgconf --libs $(PKG_CONFIG_PKGS)) $(DEBUG_FLAGS) -shared -Wl,--no-undefined

$(eval $(call define_c_targets,$(TARGET)))

all: $(TARGET)

clean: $(TARGET)_clean
	@printf "\tCLEAN\n"
