//Select board
#define v6Board
//#define v4Board
//#define maplemini

//Hardware
#define SDCard 0

#ifdef v6Board

#define MUX_BASE 100 //To differentiate with ESP32 GPIO
#define MUX_0 0 + MUX_BASE
#define MUX_1 1 + MUX_BASE
#define MUX_2 2 + MUX_BASE
#define MUX_3 3 + MUX_BASE
#define MUX_4 4 + MUX_BASE
#define MUX_5 5 + MUX_BASE
#define MUX_6 6 + MUX_BASE
#define MUX_7 7 + MUX_BASE
#define MUX_8 8 + MUX_BASE
#define MUX_9 9 + MUX_BASE
#define MUX_10 10 + MUX_BASE
#define MUX_11 11 + MUX_BASE
#define MUX_12 12 + MUX_BASE
#define MUX_13 13 + MUX_BASE
#define MUX_14 14 + MUX_BASE
#define MUX_15 15 + MUX_BASE

#define SYSTEM_SHUNT 2
#define HEATER 27
#define SD_CS MUX_2

#define PHOTOTHERAPY MUX_7
#define PWR_ALERT 2
#define SCREENBACKLIGHT 0
#define HUMIDIFIER MUX_6
#define TFT_DC 4
#define TOUCH_CS MUX_11
#define POWER_EN MUX_0
#define GPRS_EN MUX_1

#define BABY_NTC_PIN 34
#define AIR_NTC_PIN 35
#define ENC_SWITCH 33
#define BUZZER 5
#define FAN MUX_4
#define ENC_A 25
#define ENC_B 32
#define TFT_CS MUX_9
#define GPRS_PWRKEY MUX_8
#define TOUCH_IRQ MUX_10

#define TFT_RST MUX_13
#endif

#define SCREENBACKLIGHT_PWM_CHANNEL 0
#define HEATER_PWM_CHANNEL 1
#define BUZZER_PWM_CHANNEL 2
#define PWM_RESOLUTION 8
#define PWM_FREQUENCY 2000

#define maxADCvalue 4095
#define maxDACvalue 4095         //for STM32F103RE with arduino IDE
int maxPWMvalue = pow(2,PWM_RESOLUTION) - 1;

#define BL_NORMAL 0
#define BL_POWERSAVE 1

int ScreenBacklightMode;
float backlightPower;
float backlightPowerSafe;
float backlightPowerSafePercentage=0.98; //2% screen bright

int heaterMaxPWM = maxPWMvalue;
int buzzerMaxPWM = maxPWMvalue;
