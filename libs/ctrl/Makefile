################################################################################
# Project Makefile
# kyChu@2020-2-20
################################################################################

#target directory
BUILD := build
#target name
MODULE_NAME := SprinklerCtrl
#source files
C_SRCS := $(wildcard *.c)
#objects
OBJS := $(addprefix $(BUILD)/obj/,$(C_SRCS:.c=.o))
#ibject directories
OBJ_DIRS := $(sort $(dir $(OBJS)))

# External Library path
LIB_PATH = -L$(BUILD)/kylink
# External Libraries
LIBS := -lpthread -lkylink

#target
all: lib$(MODULE_NAME).so

$(OBJ_DIRS):
	$(MKDIR) -p $@

$(BUILD)/obj/%.o: %.c | $(OBJ_DIRS)
	$(ECHO) ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"

lib$(MODULE_NAME).so: $(OBJS)
	$(MAKE) $(MAKEFLAGS) libraries
	$(ECHO) ' CC $@'
	$(CC) -shared $^ -Wl,--whole-archive $(LIB_PATH) $(LIBS) -Wl,--no-whole-archive -o $(BUILD)/$@

libraries:
	$(MAKE) -C ../kylink/ BUILD=$(BUILD)/kylink

.PHONY: all
.SECONDARY: libraries
