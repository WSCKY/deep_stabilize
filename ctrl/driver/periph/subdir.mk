################################################################################
# subdir Makefile
# kyChu@2019-2-27
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./driver/periph/pwm.c \
./driver/periph/uart1.c \
./driver/periph/uart2.c \
./driver/periph/uart3.c \
./driver/periph/spi1.c \
./driver/periph/intio.c \
./driver/periph/gpio.c \
./driver/periph/irq.c

OBJS += \
$(BuildPath)/driver/periph/pwm.o \
$(BuildPath)/driver/periph/uart1.o \
$(BuildPath)/driver/periph/uart2.o \
$(BuildPath)/driver/periph/uart3.o \
$(BuildPath)/driver/periph/spi1.o \
$(BuildPath)/driver/periph/intio.o \
$(BuildPath)/driver/periph/gpio.o \
$(BuildPath)/driver/periph/irq.o

C_DEPS += \
$(BuildPath)/driver/periph/pwm.d \
$(BuildPath)/driver/periph/uart1.d \
$(BuildPath)/driver/periph/uart2.d \
$(BuildPath)/driver/periph/uart3.d \
$(BuildPath)/driver/periph/spi1.d \
$(BuildPath)/driver/periph/intio.d \
$(BuildPath)/driver/periph/gpio.d \
$(BuildPath)/driver/periph/irq.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/driver/periph/%.o: ./driver/periph/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
