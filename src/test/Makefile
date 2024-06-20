include ../../build.mk

PKG_CONFIG_PKGS:=gbm glesv2 egl

VOLUME_RENDERING_TARGET:=volume_rendering

$(VOLUME_RENDERING_TARGET)_src_files:=volume_rendering.c
$(VOLUME_RENDERING_TARGET)_cflags:=$(COMMON_CFLAGS) $(DEBUG_FLAGS) $(shell pkgconf --cflags $(PKG_CONFIG_PKGS)) -I ../libVV -I../vv
$(VOLUME_RENDERING_TARGET)_ldflags:=$(DEBUG_FLAGS) -lm $(shell pkgconf --libs $(PKG_CONFIG_PKGS)) -L../libVV -lVV
$(VOLUME_RENDERING_TARGET)_extra_obj_files:=../vv/gbm.o ../vv/bmp.o ../vv/colormap.o

$(eval $(call define_c_targets,$(VOLUME_RENDERING_TARGET)))

ISOSURFACE_TARGET:=isosurface

$(ISOSURFACE_TARGET)_src_files:=isosurface.c
$(ISOSURFACE_TARGET)_cflags:=$(COMMON_CFLAGS) $(DEBUG_FLAGS) $(shell pkgconf --cflags $(PKG_CONFIG_PKGS)) -I ../libVV -I../vv
$(ISOSURFACE_TARGET)_ldflags:=$(DEBUG_FLAGS) -lm $(shell pkgconf --libs $(PKG_CONFIG_PKGS)) -L../libVV -lVV
$(ISOSURFACE_TARGET)_extra_obj_files:=../vv/gbm.o ../vv/bmp.o

$(eval $(call define_c_targets,$(ISOSURFACE_TARGET)))

all: $(VOLUME_RENDERING_TARGET) $(ISOSURFACE_TARGET)

clean: $(VOLUME_RENDERING_TARGET)_clean $(ISOSURFACE_TARGET)_clean
	@printf "\tCLEAN\n"
