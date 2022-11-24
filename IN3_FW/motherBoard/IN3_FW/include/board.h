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

#define HW_NUM 9
#define HW_REVISION 'A'
#define HWversion String(HW_NUM) + "." + String(HW_REVISION)
#define FWversion "10.5"
#define WIFI_NAME "In3_v" + String(FWversion) + "/" + String(HWversion)
#define headingTitle "in3ator"

// Hardware
#define DISPLAY_SPI_CLK SPI_CLOCK_DIV16

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
#define ON_OFF_SWITCH 34

#define SYSTEM_SHUNT_CHANNEL INA3221_CH1
#define PHOTOTHERAPY_SHUNT_CHANNEL INA3221_CH2
#define FAN_SHUNT_CHANNEL INA3221_CH3

#define HUMIDIFIER_SHUNT 1

#define SDCard false
#define SYSTEM_SHUNT 2        // miliohms
#define PHOTOTHERAPY_SHUNT 20 // miliohms
#define FAN_SHUNT 100         // miliohms

#define HUMIDIFIER_BINARY 0
#define HUMIDIFIER_PWM 1

#define DISPLAY_CONTROLLER_IC ST7789V_CONTROLLER
#define DISPLAY_DEFAULT_ROTATION 3

#define SCREENBACKLIGHT_PWM_CHANNEL 0
#define HEATER_PWM_CHANNEL 1
#define BUZZER_PWM_CHANNEL 2
#define HUMIDIFIER_PWM_CHANNEL 7
#define DEFAULT_PWM_RESOLUTION 8
#define DEFAULT_PWM_FREQUENCY 2000
#define HEATER_PWM_FREQUENCY 2000
#define HUMIDIFIER_PWM_FREQUENCY 110000

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

#if (HW_NUM <= 8 || (HW_NUM==9 && HW_REVISION=='A') )
#define SCREEN_BRIGHTNESS_FACTOR 0.1 // Max brightness will be multiplied by this constant
#define BACKLIGHT_POWER_SAFE_PERCENTAGE 0.6
#else
#define SCREEN_BRIGHTNESS_FACTOR 0.6 // Max brightness will be multiplied by this constant
#define BACKLIGHT_POWER_SAFE_PERCENTAGE 0.1
#endif

#define BACKLIGHT_POWER_SAFE PWM_MAX_VALUE *BACKLIGHT_POWER_SAFE_PERCENTAGE
#define BACKLIGHT_POWER_DEFAULT PWM_MAX_VALUE *SCREEN_BRIGHTNESS_FACTOR
