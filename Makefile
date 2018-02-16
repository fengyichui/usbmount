CC := gcc

PREFIX  := /usr/share/usbmount
VERSION := 1.1.2
COOKIE  := /run/usbmount

INFILES = usbmount@.service usbmount config.h

SED = sed \
	  -e "s|@PREFIX@|$(PREFIX)|g" \
	  -e "s|@VERSION@|$(VERSION)|g" \
	  -e "s|@COOKIE@|$(COOKIE)|g"

all: $(INFILES) usbeject

$(INFILES): %: %.in
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

clean:
	@$(RM) *.o *.d usbeject $(INFILES)
	@echo "RM *.o *.d usbeject $(INFILES)"

.PHONY: all clean
