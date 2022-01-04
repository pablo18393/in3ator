//Select board
#define v6Board
//#define v4Board
//#define maplemini

//Hardware
#define SDCard false
#ifdef v6Board

#define TFT_DC 0
#define POWER_EN 2
#define ADC_READY 3
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
#define ENC_SWITCH 39
#define SYSTEM_SHUNT 36

#endif

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

#define ANALOG_CURRENT_SENSOR true // analog current sensing circuit is present
#define DIGITAL_CURRENT_SENSOR false

int ScreenBacklightMode;
float backlightPower;
float backlightPowerSafe;
float backlightPowerSafePercentage = 0.98; //2% screen bright

int heaterMaxPWM = maxPWMvalue;
int buzzerMaxPWM = maxPWMvalue;
