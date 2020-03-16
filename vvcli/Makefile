include ../build.mk

.SUFFIXES:
.ONESHELL:

BUILD_DIR:=./build
TARGET:=vvcli

all: $(TARGET)

clean: $(TARGET)_clean
	@echo "\tCLEAN\t$(BUILD_DIR)"
	rm -rf $(BUILD_DIR)

build:
	@echo "\tMKDIR\t$@"
	mkdir -p $@

$(TARGET)_src_files:=main.c
$(TARGET)_cflags:=-I../libVV/ -I../libVV/include/libVV
$(TARGET)_ldflags:=-L../libVV/build/ -lVV

$(eval $(call define_c_target,$(TARGET),src,$(BUILD_DIR)))
