.SUFFIXES:
.ONESHELL:
.DEFAULT_GOAL:=all

COMMON_CFLAGS:=-std=c11 -pedantic-errors -Wall -Wmissing-prototypes -Wstrict-prototypes -pipe
# DEBUG_FLAGS:=-g3 -fsanitize=address
DEBUG_FLAGS:=-g3

PROJECTS:=libVV vv test

$(PROJECTS): %:
	+@make -C src/$@ DEBUG_FLAGS="$(DEBUG_FLAGS)" COMMON_CFLAGS="$(COMMON_CFLAGS)"

vv: | libVV

test: | libVV vv

all: vv test

clean:
	@for x in $(PROJECTS)
	do
		make -C src/$$x clean
	done