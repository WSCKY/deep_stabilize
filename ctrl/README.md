# Gimbal controller

#### MCU:
```STM32F072CBT6, Cortex-M0, Freq(48MHz), ROM(128K), RAM(6K), LQFP48```
#### IMU:
```ICMM20602, 3-Axis gyroscope, 3-Axis accelerometer, QFN3x3x0.75mm package```
#### GCC:
```arm-none-eabi-gcc version 6.3.1 20170620 (release)```
#### CONNECTION:
```
     SPI11    -> ICM20602 IMU Sensor
     USART2   -> RS232 Port
     USB(CDC) -> Communicate with upper
     USART1   -> RS485 Port 1
     USART4   -> RS485 Port 2
     USART3   -> External Sensor
     CAN      -> External dirver
     TIM3_CH3 -> Heater circuit control
```

#### Hardware Resources:
```
    6-Dof IMU Sensor <ICM20602>: SPI_CS(PA4) SPI_MOSI(PA7) SPI_MISO(PA6) SPI_SCLK(PA5) INT(PC13)
    A Heater circuit: (PB0)
    LED Indicator: GREEN(PB12)
    External sensor: RX(PB11) TX(PB10)
    CAN BUS: RX(PB8) TX(PB9)
    RS232 BUS: RX(PA3) TX(PA2)
    RS485 BUS 1: RX(PB7) TX(PB6) DE(PB1)
    RS485 BUS 2: RX(PA1) TX(PA0) DE(PB2)
    USB Device: DM(PA11) DP(PA12)
    Output/Input GPIOs:
        OUTPUT1   -> PB13
        OUTPUT2   -> PB14
        OUTPUT3   -> PB15
        OUTPUT4   -> PA8
        OUTPUT5   -> PA9
        OUTPUT6   -> PA10

        INPUT1    -> PA15
        INPUT2    -> PB3
        INPUT3    -> PB4
        INPUT4    -> PB5
    RESERVED GPIOs:
        REV_GPIO1 -> PC15
        REV_GPIO2 -> PC14
```

#### DMA requests for each channel:
```
    +-------+---------+---------+-------+-------+-------+-------+
    | CHAN1 |  CHAN2  |  CHAN3  | CHAN4 | CHAN5 | CHAN6 | CHAN7 |
    +-------+---------+---------+-------+-------+-------+-------+
    |  REV  | SPI1_RX | SPI1_TX | U2_TX | U2_RX | U3_RX | U3_TX |
    +-------+---------+---------+-------+-------+-------+-------+
```

                                                       kyChu@2020/02/20

