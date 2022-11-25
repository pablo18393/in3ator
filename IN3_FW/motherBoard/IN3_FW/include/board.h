/*
  MIT License

  Copyright (c) 2022 Medical Open World, Pablo Sánchez Bergasa

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/

#define HW_NUM 6
#define HW_REVISION 'B'
#define HWversion String(HW_NUM) + "." + String(HW_REVISION)
#define FWversion "10.5"
#define WIFI_NAME "In3_v" + String(FWversion) + "/" + String(HWversion)
#define headingTitle "in3ator"

#if (HW_NUM <= 8)
#define DISPLAY_SPI_CLK SPI_CLOCK_DIV128
#else
// Hardware
#define DISPLAY_SPI_CLK SPI_CLOCK_DIV16
#endif

#if (HW_NUM <= 6)
#define HUMIDIFIER_INTERFACE HUMIDIFIER_BINARY
#define ANALOG_TO_AMP_FACTOR 0.0045
#define CURRENT_MEASURES_AMOUNT 20
#else
// Hardware
#define HUMIDIFIER_INTERFACE HUMIDIFIER_I2C
#endif

#define GPIO_EXP_BASE 100 // To differentiate with ESP32 GPIO

#if (HW_NUM == 9)
// PINOUT
#define TFT_DC 0
#define ENC_SWITCH 4
#define BUZZER 5
#define FAN 12
#define PHOTOTHERAPY 13
#define ACTUATORS_EN 14
#define TFT_CS 15
#define I2C_SDA 21
#define I2C_SCL 22
#define ENC_A 25
#define GPRS_PWRKEY 26
#define HEATER 27
#define ENC_B 32
#define SCREENBACKLIGHT 33
#define BABY_NTC_PIN 39
#define DISPLAY_CONTROLLER_IC ST7789V_CONTROLLER

// #define ON_OFF_SWITCH 34
#else
// Hardware
// PINOUT
#define TFT_DC 4
#define BUZZER 5
#define I2C_SDA 21
#define I2C_SCL 22
#define ENC_A 25
#define ENC_SWITCH 26
#define HEATER 27
#define ENC_B 32
#define SCREENBACKLIGHT 33
#define BABY_NTC_PIN 34
#define AIR_NTC_PIN 35
#define SYSTEM_CURRENT_SENSOR 36
// #define SYSTEM_SHUNT 36

#define GPIO_EXP_0 0 + GPIO_EXP_BASE
#define GPIO_EXP_1 1 + GPIO_EXP_BASE
#define GPIO_EXP_2 2 + GPIO_EXP_BASE
#define GPIO_EXP_3 3 + GPIO_EXP_BASE
#define GPIO_EXP_4 4 + GPIO_EXP_BASE
#define GPIO_EXP_5 5 + GPIO_EXP_BASE
#define GPIO_EXP_6 6 + GPIO_EXP_BASE
#define GPIO_EXP_7 7 + GPIO_EXP_BASE
#define GPIO_EXP_8 8 + GPIO_EXP_BASE
#define GPIO_EXP_9 9 + GPIO_EXP_BASE
#define GPIO_EXP_10 10 + GPIO_EXP_BASE
#define GPIO_EXP_11 11 + GPIO_EXP_BASE
#define GPIO_EXP_12 12 + GPIO_EXP_BASE
#define GPIO_EXP_13 13 + GPIO_EXP_BASE
#define GPIO_EXP_14 14 + GPIO_EXP_BASE
#define GPIO_EXP_15 15 + GPIO_EXP_BASE

#define UNUSED_GPIO_EXP0 GPIO_EXP_5
#define UNUSED_GPIO_EXP1 GPIO_EXP_12
#define UNUSED_GPIO_EXP2 GPIO_EXP_14
#define UNUSED_GPIO_EXP3 GPIO_EXP_15

#define ACTUATORS_EN GPIO_EXP_0
#define GPRS_EN GPIO_EXP_1
#define SD_CS GPIO_EXP_2
#define FAN GPIO_EXP_3
#define HUMIDIFIER GPIO_EXP_6
#define PHOTOTHERAPY GPIO_EXP_7
#define GPRS_PWRKEY GPIO_EXP_8
#define TFT_CS GPIO_EXP_9
#define TOUCH_IRQ GPIO_EXP_10
#define TOUCH_CS GPIO_EXP_11
#define TFT_RST GPIO_EXP_13

#define DISPLAY_CONTROLLER_IC ILI9341_CONTROLLER

#endif

#if (HW_NUM >= 8)
// number assignment of each enviromental sensor for later call in variable
#define skinSensor 0
#define airSensor 1
#define numNTC 1     // number of NTC
#define digitalTempHumSensor 1
#define numSensors 2 // number of total temperature sensors in system

#define DEFAULT_WIFI_EN ON
#else
// number assignment of each enviromental sensor for later call in variable
#define skinSensor 0
#define airSensor 1
#define numNTC 2     // number of NTC
#define digitalTempHumSensor 3
#define numSensors 3 // number of total temperature sensors in system

#define DEFAULT_WIFI_EN OFF
#endif

#define SYSTEM_SHUNT_CHANNEL INA3221_CH1
#define PHOTOTHERAPY_SHUNT_CHANNEL INA3221_CH2
#define FAN_SHUNT_CHANNEL INA3221_CH3

#define HUMIDIFIER_SHUNT 1

#define SDCard false
#define SYSTEM_SHUNT 2        // miliohms
#define PHOTOTHERAPY_SHUNT 20 // miliohms
#define FAN_SHUNT 100         // miliohms

#define DISPLAY_DEFAULT_ROTATION 3

#define SCREENBACKLIGHT_PWM_CHANNEL 0
#define HEATER_PWM_CHANNEL 1
#define BUZZER_PWM_CHANNEL 2
#define DEFAULT_PWM_RESOLUTION 8
#define DEFAULT_PWM_FREQUENCY 2000

#define maxADCvalue 4095
#define maxDACvalue 4095
#define PWM_MAX_VALUE (pow(2, DEFAULT_PWM_RESOLUTION) - 1)

#define minimumAllowedNTCMeasurement maxADCvalue / 5
#define maximumAllowedNTCMeasurement maxADCvalue * 4 / 5

#define BL_NORMAL 0
#define BL_POWERSAVE 1

#define maxTempToDiscard 60
#define minTempToDiscard 5

#define BUZZER_MAX_PWR PWM_MAX_VALUE
#define HEATER_HALF_PWR PWM_MAX_VALUE / 2

#define BUZZER_MAX_PWM PWM_MAX_VALUE

#if (HW_NUM <= 8 || (HW_NUM == 9 && HW_REVISION == 'A'))
#define SCREEN_BRIGHTNESS_FACTOR 0.1 // Max brightness will be multiplied by this constant
#define BACKLIGHT_POWER_SAFE_PERCENTAGE 0.6
#else
#define SCREEN_BRIGHTNESS_FACTOR 0.6 // Max brightness will be multiplied by this constant
#define BACKLIGHT_POWER_SAFE_PERCENTAGE 0.1
#endif

#define BACKLIGHT_POWER_SAFE PWM_MAX_VALUE *BACKLIGHT_POWER_SAFE_PERCENTAGE
#define BACKLIGHT_POWER_DEFAULT PWM_MAX_VALUE *SCREEN_BRIGHTNESS_FACTOR
