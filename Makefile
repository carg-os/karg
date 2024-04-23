BUILDDIR = $(CURDIR)/build

all: githooks $(BUILDDIR)/karg.a

githooks:
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

CFLAGS = -std=gnu2x -I include/ -MMD -Wall -Wextra -Werror -ffreestanding \
         -mcmodel=medany -O3 -fdata-sections -ffunction-sections -flto

$(BUILDDIR)/%.S.o: src/%.S | $(BUILDDIR)
	@printf "  CCAS\t$(@F)\n"
	@$(CC) $< -c -o $@ $(CFLAGS)

$(BUILDDIR)/%.c.o: src/%.c | $(BUILDDIR)
	@printf "  CC\t$(@F)\n"
	@$(CC) $< -c -o $@ $(CFLAGS)

$(BUILDDIR)/karg.a: $(OBJS) | $(BUILDDIR)
	@printf "  AR\t$(@F)\n"
	@$(AR) crs $@ $(OBJS)

-include $(DEPS)
