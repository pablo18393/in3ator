//Select board
#define v6Board
//#define v4Board
//#define maplemini

//Hardware
#define SPI_SEL 1
#define SDCard 0

#ifdef v6Board
#define SYSTEM_SHUNT PA0
#define PULSIOXIMETER PA1
#define HEATER PA8
#define SD_CS PA11

#define PHOTOTHERAPY PB0
#define HUMIDIFIER PB7
#define PWR_ALERT PB4
#define PULSIOXIMETER_EN PB5
#define SCREENBACKLIGHT PB6
#define TFT_DC PB8
#define TOUCH_CS PB9
#define POWER_EN PB12

#define BABY_NTC_PIN PC1
#define ENC_SWITCH PC5
#define BUZZER PC6
#define FAN PC7
#define ENC_A PC8
#define ENC_B PC9
#define TFT_CS PC13
#define GPRS_PWRKEY PC14
#define TOUCH_IRQ PC15

#define TFT_RST PD2
#endif


#define maxADCvalue 4095
#define maxDACvalue 4095         //for STM32F103RE with arduino IDE
#define maxPWMvalue 65535
float backlightPower;
int heaterMaxPWM;
int buzzerMaxPWM;
