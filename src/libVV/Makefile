include ../../build.mk

TARGET:=libVV.so

PKG_CONFIG_PKGS:=glesv2 egl

$(TARGET)_src_files:=gles.c isosurface.c isosurface_renderer.c matrix.c raw.c renderer.c vertex_buffer.c volume.c volume_renderer.c volume_texture.c
$(TARGET)_resource_files:=volume.vert volume.frag isosurface.vert isosurface.frag
$(TARGET)_cflags:=$(COMMON_CFLAGS) $(DEBUG_FLAGS) $(shell pkgconf --cflags $(PKG_CONFIG_PKGS)) -fPIC
$(TARGET)_ldflags:=$(DEBUG_FLAGS) -lm $(shell pkgconf --libs $(PKG_CONFIG_PKGS)) -shared -Wl,--no-undefined

$(eval $(call define_c_targets,$(TARGET)))

all: $(TARGET)

clean: $(TARGET)_clean
	@printf "\tCLEAN\n"
