module := $(shell basename $(CURDIR))

srcs := $(wildcard $(SRCDIR)/*.c)
objs := $(srcs:%.c=$(builddir)/$(module)/%.o)
deps := $(objs:%.o=%.d)

basetarget = $(1:$(builddir)/%=%)

$(builddir)/$(module)/%.o: %.c $(builddir)/$(module)/%.d
	@mkdir -p $(dir $@)
	@echo -e "  CC      $(call basetarget,$@)"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

$(builddir)/$(module)/%.d: %.c
	@mkdir -p $(dir $@)
	$(Q)$(CC) $(CFLAGS) -MM -MT $(@:%.d=%.o) $< > $@

-include $(deps)
