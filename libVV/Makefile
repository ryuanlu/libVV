include ../build.mk

.SUFFIXES:
.ONESHELL:

BUILD_DIR:=./build
LINKER_NAME:=libVV.so
ABI_VERSION:=0
TARGET:=$(LINKER_NAME).$(ABI_VERSION)
TEST_TARGET:=test

all: $(BUILD_DIR)/$(LINKER_NAME) $(BUILD_DIR)/$(TEST_TARGET)

clean: $(TARGET)_clean
	@echo "\tCLEAN\t$(BUILD_DIR)"
	rm -rf $(BUILD_DIR)

build:
	@echo "\tMKDIR\t$@"
	mkdir -p $@

$(TARGET)_src_files:=context.c gles.c cl.c memory.c debug.c
$(TARGET)_cflags:=-fPIC -Wall -Iinclude/libVV -g
$(TARGET)_ldflags:=-shared -Wall -Wl,-soname,$(TARGET) -lEGL -lGLESv2 -lOpenCL -g

$(eval $(call define_c_target,$(TARGET),src,$(BUILD_DIR)))

$(BUILD_DIR)/$(LINKER_NAME): $(BUILD_DIR)/$(TARGET)
	@echo "\tLN\t$@"
	ln -sf $(<F) $@

$(TEST_TARGET)_src_files:=test.c
$(TEST_TARGET)_cflags:=-Wall -Iinclude/libVV -g
$(TEST_TARGET)_ldflags:=-Wall -Lbuild/ -lVV -g

$(eval $(call define_c_target,$(TEST_TARGET),src,$(BUILD_DIR)))

$(BUILD_DIR)/$(TEST_TARGET): | $(BUILD_DIR)/$(LINKER_NAME)

run_test: $(TEST_TARGET)
	@LD_LIBRARY_PATH=$(BUILD_DIR)/ $(BUILD_DIR)/$(TEST_TARGET)
