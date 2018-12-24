CC := gcc

PREFIX  := /usr/share/usbmount
VERSION := 1.1.5
COOKIE  := /run/usbmount

FILES = usbmount@.service usbmount config.h

SED = sed \
	  -e "s|@PREFIX@|$(PREFIX)|g" \
	  -e "s|@VERSION@|$(VERSION)|g" \
	  -e "s|@COOKIE@|$(COOKIE)|g"

all: $(FILES) usbeject

$(FILES): %: %.in
	@$(SED) $< > $@
	@echo "GEN $@"

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

.PHONY: all clean
