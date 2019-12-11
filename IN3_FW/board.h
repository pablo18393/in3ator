//Select board
#define v4Board
//#define maplemini


#ifdef v4Board
#define DHTPIN PC10
#define SCREENBACKLIGHT PB4
word HEATER_NTC_PIN = PC1; //This is not #define because can be swaped with BABY_NTC_PIN
word BABY_NTC_PIN = PB1;   //This is not #define because can be swaped with HEATER_NTC_PIN
#define INBOARD_LEFT_NTC_PIN PC2
#define INBOARD_RIGHT_NTC_PIN PC3
#define ENC_A PC8
#define ENC_B PC9
#define ENC_ENC_SWITCH PC6
#define POWER_EN PC11
#define FAN_HP PA8
#define FAN_LP PB11
#define HEATER PA4 //PA4 pin
#define JAUNDICE PB0
#define STERILIZE_CTL PA15
#define HUMIDIFIER PB14
#define HEATER_FB PA0
#define POWER_EN_FB PC12
#define FAN_HP_FB PB5
#define FAN_LP_FB PB2
#define JAUNDICE_FB PC4
#define STERILIZE_FB PC5
#define HUMIDIFIER_FB PB13
#define ENC_SWITCH PC6
#define GPRS_PWRKEY PC14
#define BME_CS PA2
#define THERMISTOR_INBOARD_LEFT PC3 //PC3
#define THERMISTOR_INBOARD_RIGHT PC2 //PC2
#define SYSTEM_SHUNT PA4
#define PULSIOXIMETER 7
#define GPRSRXPIN PA10
#define BUZZER PC7
// Use hardware SPI lines+
//Pins from 4-6 are SPI
#define TFT_CS PC13
#define TFT_DC PB8
#define TFT_RST PD2
#define SD_CS PB15
#endif

//Hardware
#define DHTSensor 1
#define BME280Sensor 0
#define SPI_SEL 1
#define SDCard 0

#ifdef maplemini
//pin declaration
//boardPWMPins: 3, 4, 5, 8, 9, 10, 11, 15, 16, 25, 26, 27
#define DHTPIN 0
#define SCREENBACKLIGHT 3
int HEATER_NTC_PIN = 10;
int BABY_NTC_PIN = 11;
#define INBOARD_LEFT_NTC_PIN 11
#define INBOARD_RIGHT_NTC_PIN 11
#define ENC_A 12
#define ENC_B 13
#define POWER_EN 18
#define FAN_HP 9
#define FAN_LP 8
#define BME_CS 15
#define HEATER 16
#define SYSTEM_SHUNT PA4
#define JAUNDICE 25
#define STERILIZE_CTL 28
#define HUMIDIFIER 27
#define HEATER_FB 17
#define POWER_EN_FB 19
#define FAN_HP_FB 20
#define FAN_LP_FB 21
#define GPRSRXPIN 25
#define JAUNDICE_FB 26
#define STERILIZE_FB 29
#define HUMIDIFIER_FB 30
#define ENC_SWITCH 14
#define GPRS_PWRKEY 1
#define PULSIOXIMETER 1
// Use hardware SPI lines+
//Pins from 4-6 are SPI
#define TFT_CS         7
#define TFT_DC         31
#define TFT_RST        2
#define SD_CS PB15
#endif

#define maxADCvalue 4095
#define maxPWMvalue 255         //for STM32F103RE with arduino IDE
#define maxDACvalue 4095         //for STM32F103RE with arduino IDE
