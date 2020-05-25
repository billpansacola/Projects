# Target library
lib		:= libfs.a
targets := $(lib)
objs	:= fs.o disk.o

CC		:= gcc
CFLAGS  := -Wall -Werror
ifneq ($(D),1)
CFLAGS	+= -O2
else
CFLAGS	+= -O0
CFLAGS	+= -g
endif
PANDOC  := pandoc

ifneq ($(V),1)
Q=@
endif

all: $(targets)

# Dep tracking *must* be below the 'all' rule
deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)
DEPFLAGS = -MMD -MF $(@:.o=.d)

## TODO: Phase 1

%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $< $(DEPFLAGS)


$(lib): $(objs)
	ar -crs $(lib) $^

clean:
	@echo "CLEAN"
	$(Q)rm -f $(targets) $(objs) $(deps)
