//Hardware
#define SDCard false

#define HUMIDIFIER_BINARY 0
#define HUMIDIFIER_PWM 1

#define DISPLAY_CONTROLLER_IC ST7789V_CONTROLLER
#define DISPLAY_DEFAULT_ROTATION 3
#define HUMIDIFIER_MODE HUMIDIFIER_PWM

#define MAIN_SHUNT 0
#define HUMIDIFIER_SHUNT 1
#define currentSensingNum 2

#define TFT_DC 0
#define ADC_READY 2
#define ENC_SWITCH 4
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
#define AIR_NTC_PIN 34
#define USB_SHUNT 35
#define SYS_SHUNT 36
#define BABY_NTC_PIN 39

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

int HUMIDIFIER_DUTY_CYCLE = 0.40 * PWM_MAX_VALUE;

#define BL_NORMAL 0
#define BL_POWERSAVE 1

#define maxTempToDiscard 60
#define minTempToDiscard 5

int ScreenBacklightMode;
float backlightPower;
float backlightPowerSafe;
float backlightPowerSafePercentage = 0.98; //2% screen bright

int heaterMaxPWM = PWM_MAX_VALUE;
int buzzerMaxPWM = PWM_MAX_VALUE;
