include ../build.mk

.SUFFIXES:
.ONESHELL:

BUILD_DIR:=./build
LINKER_NAME:=libVV.so
ABI_VERSION:=0
TARGET:=$(LINKER_NAME).$(ABI_VERSION)

all: $(BUILD_DIR)/$(LINKER_NAME)

clean: $(TARGET)_clean
	@echo "\tCLEAN\t$(BUILD_DIR)"
	rm -rf $(BUILD_DIR)

$(TARGET)_src_files:=context.c gles.c cl.c memory.c
$(TARGET)_cflags:=-fPIC -Wall -Iinclude/libVV -g
$(TARGET)_ldflags:=-shared -Wall -Wl,-soname,$(TARGET) -lEGL -lGLESv2 -lOpenCL -g

$(eval $(call define_c_target,$(TARGET),src,$(BUILD_DIR)))

$(BUILD_DIR)/$(LINKER_NAME): $(BUILD_DIR)/$(TARGET)
	@echo "\tLN\t$@"
	ln -sf $(<F) $@
