CC := gcc

PREFIX  := /usr/share/usbmount
VERSION := 1.1.5
COOKIE  := /run/usbmount

all: $(FILES) usbeject

CFLAGS := -MMD -MP -g

SRCS := $(usbeject.c)
DEPS := $(SRCS:.c=.c.d)
OBJS := $(SRCS:.c=.c.o)


usbeject: usbeject.c.o
	@$(CC) -o $@ $^ $(LDFLAGS)
	@echo "GEN $@"

%.c.o: %.c
	@$(CC) -o $@ -c $^ $(CFLAGS)
	@echo "CC  $@"
-include $(DEPS)

define rm
	@$(RM) $(1)
	@echo "RM $(1)"

endef
clean:
	$(foreach f,$(sort $(wildcard *.[do]) usbeject $(FILES)),$(call rm,$(f)))

build:
	@sudo dpkg-buildpackage -us -uc -b

.PHONY: all clean build
