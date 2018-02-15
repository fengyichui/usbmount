APP := usbeject
CC := gcc


all: $(APP)

CFLAGS := -MMD -MP

SRCS := $(usbeject.c)
DEPS := $(SRCS:.c=.c.d)
OBJS := $(SRCS:.c=.c.o)

$(APP): $(APP).c.o
	@$(CC) -o $@ $^ $(LDFLAGS)
	@echo "GEN $@"

%.c.o: %.c
	@$(CC) -o $@ -c $^ $(CFLAGS)
	@echo "CC  $@"
-include $(DEPS)

clean:
	$(RM) *.o *.d $(APP)
	@echo "RM *.o *.d $(APP)"

.PHONY: all clean
