################################################################################
# subdir Makefile
# kyChu@2019-2-27
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./driver/device/rs485.c \
./driver/device/servo.c \
./driver/device/encoder.c

OBJS += \
$(BuildPath)/driver/device/rs485.o \
$(BuildPath)/driver/device/servo.o \
$(BuildPath)/driver/device/encoder.o

C_DEPS += \
$(BuildPath)/driver/device/rs485.d \
$(BuildPath)/driver/device/servo.d \
$(BuildPath)/driver/device/encoder.d

ifeq ($(CONFIG_USE_BOARD_IMU),y)
C_SRCS += ./driver/device/icm20602.c

OBJS += $(BuildPath)/driver/device/icm20602.o

C_DEPS += $(BuildPath)/driver/device/icm20602.d
endif

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/driver/device/%.o: ./driver/device/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
