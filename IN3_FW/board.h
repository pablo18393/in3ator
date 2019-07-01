
// Use hardware SPI lines+
#define TFT_CS PC13
#define TFT_DC PB8
#define TFT_RST PD2
#define DHTPIN PC10
#define SCREENBACKLIGHT PB4
word THERMISTOR_HEATER = PB1; //This is not #define because can be swaped with THERMISTOR_ROOM
word THERMISTOR_ROOM = PC1;   //This is not #define because can be swaped with THERMISTOR_HEATER
#define ENC_A PC8
#define ENC_B PC9
#define ENC_ENC_PULSE PC6
#define POWER_EN PC11
#define FAN_HP PB8
#define FAN_LP PB11
#define HEATER PA4
#define JAUNDICE PB0
#define STERILIZE PA15
#define HUMIDIFIER PB14
#define HEATER_FB PA0
#define POWER_EN_FB PC12
#define FAN_HP_FB PB5
#define FAN_LP_FB PB2
#define JAUNDICE_FB PC4
#define STERILIZER_FB PC5
#define HUMIDIFIER_FB PB13
#define ENC_PULSE PC6


#define THERMISTOR_INBOARD_LEFT 10 //PC3
#define THERMISTOR_INBOARD_RIGHT 11 //PC2
#define PULSIOXIMETER 7

//pin declaration
//boardPWMPins: 3, 4, 5, 8, 9, 10, 11, 15, 16, 25, 26, 27
/*
  // Use hardware SPI lines+
  //Pins from 4-6 are SPI
  #define TFT_CS PC13
  #define TFT_DC PB8
  #define TFT_RST PD2

  #define DHTPIN 0
  #define SCREENBACKLIGHT PB4
  byte THERMISTOR_HEATER = 10;
  byte THERMISTOR_ROOM = 11;
  #define ENC_A PC8
  #define ENC_B PC9
  #define POWER_EN 18
  #define FAN_HP 9
  #define FAN_LP 8
  #define FAN_EXTRA 15
  #define HEATER 16
  #define GENERIC 16
  #define JAUNDICE 25
  #define STERILIZE 28
  #define HUMIDIFIER 27
  #define HEATER_FB 17
  #define POWER_EN_FB 19
  #define FAN_HP_FB 20
  #define FAN_LP_FB 21
  #define FAN_EXTRA_FB 22
  #define GENERIC_FB 22
  #define JAUNDICE_FB 26
  #define STERILIZER_FB 29
  #define HUMIDIFIER_FB 30
  #define ENC_PULSE PC6
*/
#define maxADCvalue 4098
#define maxPWMvalue 255         //for maple mini with arduino IDE
