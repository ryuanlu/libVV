
PREFIX?=/usr

TARGETS:=vvcli libVV

.SUFFIXES:
.ONESHELL:

.PHONY: $(TARGETS)

all: $(TARGETS)

clean:
	+@for target in ${TARGETS}
	do
		make -C $${target} $@
	done

install: all
	+@echo "\tINSTALL\t$(DESTDIR)$(PREFIX)"

$(TARGETS): %:
	+@make -C $@

vvcli: libVV

