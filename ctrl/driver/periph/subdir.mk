################################################################################
# subdir Makefile
# kyChu@2019-2-27
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./driver/periph/pwm.c \
./driver/periph/pwm16.c \
./driver/periph/pwm17.c \
./driver/periph/uart1.c \
./driver/periph/uart2.c \
./driver/periph/uart3.c \
./driver/periph/uart4.c \
./driver/periph/tim7.c \
./driver/periph/gpio.c \
./driver/periph/irq.c

OBJS += \
$(BuildPath)/driver/periph/pwm.o \
$(BuildPath)/driver/periph/pwm16.o \
$(BuildPath)/driver/periph/pwm17.o \
$(BuildPath)/driver/periph/uart1.o \
$(BuildPath)/driver/periph/uart2.o \
$(BuildPath)/driver/periph/uart3.o \
$(BuildPath)/driver/periph/uart4.o \
$(BuildPath)/driver/periph/tim7.o \
$(BuildPath)/driver/periph/gpio.o \
$(BuildPath)/driver/periph/irq.o

C_DEPS += \
$(BuildPath)/driver/periph/pwm.d \
$(BuildPath)/driver/periph/pwm16.d \
$(BuildPath)/driver/periph/pwm17.d \
$(BuildPath)/driver/periph/uart1.d \
$(BuildPath)/driver/periph/uart2.d \
$(BuildPath)/driver/periph/uart3.d \
$(BuildPath)/driver/periph/uart4.d \
$(BuildPath)/driver/periph/tim7.d \
$(BuildPath)/driver/periph/gpio.d \
$(BuildPath)/driver/periph/irq.d

ifeq ($(CONFIG_USE_BOARD_IMU),y)
C_SRCS += ./driver/periph/spi1.c \
./driver/periph/intio.c

OBJS += $(BuildPath)/driver/periph/spi1.o  \
$(BuildPath)/driver/periph/intio.o

C_DEPS += $(BuildPath)/driver/periph/spi1.d \
$(BuildPath)/driver/periph/intio.d
endif

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/driver/periph/%.o: ./driver/periph/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
