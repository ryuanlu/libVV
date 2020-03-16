define define_c_target
$(1)_obj_files:=$$(foreach obj,$$($(1)_src_files:.c=.o),$(3)/$$(obj))
$(1)_dep_files:=$$($(1)_obj_files:.o=.o.d)

$$($(1)_obj_files): $(3)/%.o: $(2)/%.c | $(3)
	@echo "\tCC\t$$@"
	$$(CC) -MMD -MF $$@.d -c $$< -o $$@ $$($(1)_cflags)

$(1): $(3)/$(1)

$(3)/$(1): $$($(1)_obj_files)
	@echo "\tLD\t$$@"
	$$(CC) $$^ -o $$@ $$($(1)_ldflags)

$(1)_clean:
	@echo "\tCLEAN\t$(1)"
	rm -f $$($(1)_obj_files) $(3)/$(1) $(3)/$(1).d

sinclude $$($(1)_dep_files)

endef