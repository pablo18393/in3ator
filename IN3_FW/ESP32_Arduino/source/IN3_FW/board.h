//Hardware
#define SDCard false

#define DISPLAY_CONTROLLER_IC ST7789V_CONTROLLER
#define DISPLAY_DEFAULT_ROTATION 3

#define TFT_DC 0
#define POWER_EN 2
#define ADC_READY 4
#define BUZZER 5
#define FAN 12
#define PHOTOTHERAPY 13
#define HUMIDIFIER 14
#define TFT_CS 15
#define ENC_A 25
#define GPRS_PWRKEY 26
#define HEATER 27
#define ENC_B 32
#define SCREENBACKLIGHT 33
#define BABY_NTC_PIN 34
#define AIR_NTC_PIN 35
#define SYSTEM_SHUNT 36
#define ENC_SWITCH 39

#define SCREENBACKLIGHT_PWM_CHANNEL 0
#define HEATER_PWM_CHANNEL 1
#define BUZZER_PWM_CHANNEL 2
#define PWM_RESOLUTION 8
#define PWM_FREQUENCY 2000

#define maxADCvalue 4095
#define maxDACvalue 4095         //for STM32F103RE with arduino IDE
int maxPWMvalue = pow(2, PWM_RESOLUTION) - 1;

#define BL_NORMAL 0
#define BL_POWERSAVE 1

int ScreenBacklightMode;
float backlightPower;
float backlightPowerSafe;
float backlightPowerSafePercentage = 0.98; //2% screen bright

int heaterMaxPWM = maxPWMvalue;
int buzzerMaxPWM = maxPWMvalue;
