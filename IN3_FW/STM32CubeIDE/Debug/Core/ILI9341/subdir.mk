################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/ILI9341/ILI9341_GFX.c \
../Core/ILI9341/ILI9341_STM32_Driver.c \
../Core/ILI9341/ILI9341_Touchscreen.c 

OBJS += \
./Core/ILI9341/ILI9341_GFX.o \
./Core/ILI9341/ILI9341_STM32_Driver.o \
./Core/ILI9341/ILI9341_Touchscreen.o 

C_DEPS += \
./Core/ILI9341/ILI9341_GFX.d \
./Core/ILI9341/ILI9341_STM32_Driver.d \
./Core/ILI9341/ILI9341_Touchscreen.d 


# Each subdirectory must supply rules for building sources it contributes
Core/ILI9341/ILI9341_GFX.o: ../Core/ILI9341/ILI9341_GFX.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32F103xE -DUSE_HAL_DRIVER -DDEBUG -c -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I"C:/Users/pablo/Desktop/IN3/in3/IN3_FW/STM32CubeIDE/Core/ILI9341" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/ILI9341/ILI9341_GFX.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Core/ILI9341/ILI9341_STM32_Driver.o: ../Core/ILI9341/ILI9341_STM32_Driver.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32F103xE -DUSE_HAL_DRIVER -DDEBUG -c -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I"C:/Users/pablo/Desktop/IN3/in3/IN3_FW/STM32CubeIDE/Core/ILI9341" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/ILI9341/ILI9341_STM32_Driver.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Core/ILI9341/ILI9341_Touchscreen.o: ../Core/ILI9341/ILI9341_Touchscreen.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32F103xE -DUSE_HAL_DRIVER -DDEBUG -c -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I"C:/Users/pablo/Desktop/IN3/in3/IN3_FW/STM32CubeIDE/Core/ILI9341" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/ILI9341/ILI9341_Touchscreen.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

