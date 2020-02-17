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
     USART2   -> Communicate with upper
     USB(CDC) -> Communicate with upper
     USART1   -> RS485 Port 1
     USART4   -> RS485 Port 2
     USART3   -> External Sensor
     CAN      -> External dirver
     TIM3_CH3 -> Heater circuit control
```

#### Hardware Resources:
```
    6-Dof IMU Sensor <ICM20602>:
        The ICM-20602 is a 6-axis MotionTracking device that combines a 3-axis gyroscope, 3-axis accelerometer, in a small 3 mm x 3 mm x 0.75 mm (16-pin LGA) package.
    A Heater circuit:
        provides a constant temperature envrionment for the IMU sensor.
```

                                                       kyChu@2020/02/20

