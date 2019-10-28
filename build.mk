define define_c_target
$(1)_obj_files:=$$(foreach obj,$$($(1)_src_files:.c=.o),$(3)/$$(obj))

$$($(1)_obj_files): $(3)/%.o: $(2)/%.c
	@echo "\tCC\t$$@"
	$$(CC) -c $$< -o $$@ $$($(1)_cflags)

$(1): $(3)/$(1)

$(3)/$(1): $$($(1)_obj_files) | $(3)
	@echo "\tLD\t$$@"
	$$(CC) $$^ -o $$@ $$($(1)_ldflags)

$(3)/$(1).d: $$(foreach src,$$($(1)_src_files),$(2)/$$(src)) | $(3)
	@echo "\tDEP\t$$@"
	$$(CC) -MM $$^ $$($(1)_cflags) > $$@

$(1)_clean:
	@echo "\tCLEAN\t$(1)"
	rm -f $$($(1)_obj_files) $(3)/$(1) $(3)/$(1).d

$(3):
	@echo "\tMKDIR\t$$@"
	mkdir -p $$@

sinclude $(3)/$(1).d

endef