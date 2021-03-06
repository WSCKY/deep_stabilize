################################################################################
# subdir Makefile
# kyChu@2019-3-9
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./ctrl/uart.c \
./ctrl/SprinklerCtrl.c 

OBJS += \
$(BuildPath)/ctrl/uart.o \
$(BuildPath)/ctrl/SprinklerCtrl.o 

C_DEPS += \
$(BuildPath)/ctrl/uart.d \
$(BuildPath)/ctrl/SprinklerCtrl.d 

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/ctrl/%.o: ./ctrl/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
