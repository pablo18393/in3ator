//Select board
#define v6Board
//#define v4Board
//#define maplemini

//Hardware
#define SDCard 0

#ifdef v6Board

#define GPIO_EXP_BASE 100 //To differentiate with ESP32 GPIO
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

#define SYSTEM_SHUNT 2
#define HEATER 27
#define SD_CS GPIO_EXP_2

#define PHOTOTHERAPY GPIO_EXP_7
#define PWR_ALERT 2
#define SCREENBACKLIGHT 0
#define HUMIDIFIER GPIO_EXP_6
#define TFT_DC 4
#define TOUCH_CS GPIO_EXP_11
#define POWER_EN GPIO_EXP_0
#define GPRS_EN GPIO_EXP_1

#define BABY_NTC_PIN 34
#define AIR_NTC_PIN 35
#define ENC_SWITCH 33
#define BUZZER 5
#define FAN GPIO_EXP_4
#define ENC_A 25
#define ENC_B 32
#define TFT_CS GPIO_EXP_9
#define GPRS_PWRKEY GPIO_EXP_8
#define TOUCH_IRQ GPIO_EXP_10

#define TFT_RST GPIO_EXP_13
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
