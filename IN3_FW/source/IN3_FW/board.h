//Hardware


//PINOUT
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
#define VOLTAGE_IN 35
#define USB_SHUNT 35
#define SYS_SHUNT 35
#define BABY_NTC_PIN 39

#define SYSTEM_SHUNT_CHANNEL INA3221_CH1
#define PHOTOTHERAPY_SHUNT_CHANNEL INA3221_CH2
#define FAN_SHUNT_CHANNEL INA3221_CH3

#define HUMIDIFIER_SHUNT 1

#define SDCard false
#define SYSTEM_SHUNT 3 //miliohms
#define PHOTOTHERAPY_SHUNT 20 //miliohms
#define FAN_SHUNT 100 //miliohms

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
#define HEATER_PWM_FREQUENCY 100
#define HUMIDIFIER_PWM_FREQUENCY 110000

#define maxADCvalue 4095
#define maxDACvalue 4095
#define PWM_MAX_VALUE (pow(2, DEFAULT_PWM_RESOLUTION) - 1)

#define minimumAllowedNTCMeasurement maxADCvalue/5
#define maximumAllowedNTCMeasurement maxADCvalue*4/5

int HUMIDIFIER_DUTY_CYCLE = 0.40 * PWM_MAX_VALUE;

#define BL_NORMAL 0
#define BL_POWERSAVE 1

#define maxTempToDiscard 60
#define minTempToDiscard 5

int ScreenBacklightMode;
float backlightPower;
float backlightPowerSafe;
float backlightPowerSafePercentage = 0.1; //10% screen bright
const float screenBrightnessFactor = 0.6; //Max brightness will be multiplied by this constant

#define BUZZER_MAX_PWR PWM_MAX_VALUE
#define HEATER_HALF_PWR PWM_MAX_VALUE/2

#define BUZZER_MAX_PWM PWM_MAX_VALUE
