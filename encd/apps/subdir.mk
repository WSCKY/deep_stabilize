################################################################################
# subdir Makefile
# kyChu@2019-2-20
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./apps/main_task.c \
./apps/sins_task.c \
./apps/ctrl_task.c \
./apps/stat_task.c \
./apps/encd_task.c \
./apps/parameter.c \
./apps/com_task.c \
./apps/cpu_utils.c \
./apps/printHardFault.c

OBJS += \
$(BuildPath)/apps/main_task.o \
$(BuildPath)/apps/sins_task.o \
$(BuildPath)/apps/ctrl_task.o \
$(BuildPath)/apps/stat_task.o \
$(BuildPath)/apps/encd_task.o \
$(BuildPath)/apps/parameter.o \
$(BuildPath)/apps/com_task.o \
$(BuildPath)/apps/cpu_utils.o \
$(BuildPath)/apps/printHardFault.o

C_DEPS += \
$(BuildPath)/apps/main_task.d \
$(BuildPath)/apps/sins_task.d \
$(BuildPath)/apps/ctrl_task.d \
$(BuildPath)/apps/stat_task.d \
$(BuildPath)/apps/encd_task.d \
$(BuildPath)/apps/com_task.d \
$(BuildPath)/apps/cpu_utils.d \
$(BuildPath)/apps/printHardFault.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/apps/%.o: ./apps/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
