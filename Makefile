BUILDDIR = $(CURDIR)/build

all: githooks $(BUILDDIR)/karg.elf

FORCE:

githooks: | hooks
	@git config core.hooksPath hooks/

$(BUILDDIR):
	@mkdir $(BUILDDIR)

clean:
	@$(RM) -rf $(BUILDDIR)

CC = $(CROSS_PREFIX)gcc

SRCS = $(wildcard src/*.S src/*.c)
OBJS = $(patsubst src/%.S,$(BUILDDIR)/%.S.o,$(SRCS))
OBJS := $(patsubst src/%.c,$(BUILDDIR)/%.c.o,$(OBJS))
DEPS = $(patsubst %.o,%.d,$(OBJS))

CFLAGS = -std=gnu2x -I include/ -MMD -Wall -Wextra -Werror -ffreestanding -mcmodel=medany
LDFLAGS = -nostdlib

$(BUILDDIR)/%.S.o: src/%.S | $(BUILDDIR)
	@printf "  CCAS\t$(@F)\n"
	@$(CC) $< -c -o $@ $(CFLAGS)

$(BUILDDIR)/%.c.o: src/%.c | $(BUILDDIR)
	@printf "  CC\t$(@F)\n"
	@$(CC) $< -c -o $@ $(CFLAGS)

init:
	@git clone https://github.com/carg-os/init.git

$(BUILDDIR)/init.a: FORCE | init $(BUILDDIR)
	@$(MAKE) -C init BUILDDIR=$(BUILDDIR)

$(BUILDDIR)/karg.elf: $(OBJS) $(BUILDDIR)/init.a kernel.ld | $(BUILDDIR)
	@printf "  CCLD\t$(@F)\n"
	@$(CC) $(OBJS) $(BUILDDIR)/init.a -o $@ -T kernel.ld $(LDFLAGS)

run: $(BUILDDIR)/karg.elf
	@qemu-system-riscv64 -M virt -nographic -kernel $<

-include $(DEPS)
