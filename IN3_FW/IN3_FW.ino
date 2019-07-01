#include <Adafruit_GFX_AS.h>
#include <EEPROM.h>
#include <Adafruit_ILI9341_STM.h> // STM32 DMA Hardware-specific library
#include <SPI.h>
#include "DHT.h"
#include <PID_v1.h>
#include "board.h"

#define FWversion "v2.3"
#define headingTitle "in3ator"

//configuration variables
#define debounceTime 100        //encoder debouncing time
#define mosfet_switch_time 100   //in millis, oversized
#define timePressToSettings 5000 //in millis, time to press to go to settings window

//pages
#define menuPage 0
#define actuatorsProgressPage 1
#define settingsPage 2
#define calibrateSensorsPage 3
#define askSuccessPage 4

//languages
#define firstLanguage 1
#define spanish 1
#define english 2
#define french 3
#define numLanguages 3

//temperature variables
#define CheckSensorRaiseTemp 3   //in celsius degrees, the amount of degrees to differentiate heater or room NTC
#define roomNTC 0
#define heaterNTC 1
#define inBoardLeftNTC 2
#define inBoardRightNTC 3
#define dhtSensor 1
#define numNTC 4
#define numTempSensors 5
#define temperature_fraction 20 //numbers of measures of the temperatures sensors


int temperatureArray [numNTC][temperature_fraction];
bool faultNTC[numNTC];
byte heaterPower;
byte numSensors;
double temperature[numTempSensors];
double previousTemperature[numTempSensors];
int humidity;
int previousHumidity;

//Sensor check rate (in ms)
const byte pulsioximeterRate = 5;
byte pulsioximeterCount = 0;
byte encoderRate = 1;
byte encoderCount = 0;

//GPRS variables
#define aliveRefresh 0
#define environmental 1
#define ENC_PULSESensorVariables 2
#define ENC_PULSESensorRaw 3
#define turnedOn 4


//sensor variables
int pulsioximeterMean;
const int maxPulsioximeterSamples = 320; //(tft width).
int pulsioximeterSample[maxPulsioximeterSamples][2]; //0 is previous data, 1 is actual data
int pulsioximeterPeakThreshold;
int pulsioximeterMaxPeak;
int pulsioximeterMinPeak;
long pulsioximeterMinPeakTime;
long pulsioximeterMaxPeakTime;
int minPulsioximeterCoordinate;
int maxPulsioximeterCoordinate;
const bool pulsioximeterDrawn = 0;
const bool pulsioximeterSampled = 1;
int pulsioximeterInterSamples[pulsioximeterRate];
int pulsioximeterMultiplierFilterFactor = 1000;
int pulsioximeterRestFilterFactor = 1;
int pulsioximeterAmplitud;
int pulsioximeterCounter[2];

//constants
const byte heaterMaxTemp = 100;
const byte minTemp = 15;
const byte maxTemp = 45;
const byte maxHum = 100;
const byte minHum = 20;

//Encoder variables
#define NUMENCODERS 1
byte NTCpin[numNTC] = {THERMISTOR_HEATER, THERMISTOR_ROOM, THERMISTOR_INBOARD_LEFT, THERMISTOR_INBOARD_RIGHT};
volatile int encstate[NUMENCODERS];
volatile int encflag[NUMENCODERS];
boolean A_set[NUMENCODERS];
boolean B_set[NUMENCODERS];
volatile int16_t encoderpos[NUMENCODERS];
volatile int  encodertimer = millis(); // acceleration measurement
int encoderpinA[NUMENCODERS] = {ENC_A}; // pin array of all encoder A inputs
int encoderpinB[NUMENCODERS] = {ENC_B}; // pin array of all encoder B inputs
unsigned int lastEncoderPos[NUMENCODERS];
int EncOldPosition;
int EncNewPosition;

//display graphic geometric variables
#define introDelay    1000      //initial delay between intro and menu
#define brightenRate  30        //intro brighten speed (Higher value, slower)
#define valuePosition 245
#define separatorPosition 240
#define unitPosition 315
#define textFontSize 4          //text standard size
#define helpTextMenuCentreX tft.width()/2;
#define width_select  7
#define height_heading  34
#define width_indentation  4
#define width_back  50
#define side_gap  4
#define letter_height  26
#define letter_width  14
#define triang  6
#define radius  12
#define circle  8
#define logo  40
#define battery_lenght  50
#define battery_height  6
#define battery_gap  2
#define battery_margin  20
#define battery_round  4
#define arrow_height  6
#define arrow_tail  5
//graphic positions
#define graphJAUNDICEextOffset 1                //bar pos is counted from 1, but text from 0
//menu
#define centered 1
#define leftMargin 0
#define temperatureGraphicPosition 0
#define humidityGraphicPosition 1
#define LEDGraphicPosition 2
#define settingsGraphicPosition 4
#define startGraphicPosition 3
//settings
#define autoLockGraphicPosition 0
#define languageGraphicPosition 1
#define heaterTempGraphicPosition 2
#define fanGraphicPosition 3
#define setStandardValuesGraphicPosition 4
#define calibrateGraphicPosition 5
//calibration
#define temperatureCalibrationGraphicPosition 0
#define humidityCalibrationGraphicPosition 1
#define restartCalibrationValuesTempGraphicPosition 2
//askSuccess
#define successQuestionGraphicPosition 0
#define afirmativeGraphicPosition 1
#define negativeGraphicPosition 2
//colors
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define COLOR_WARNING_TEXT ILI9341_ORANGE
#define COLOR_MENU BLACK
#define COLOR_BAR  BLACK
#define COLOR_MENU_TEXT WHITE
#define COLOR_SELECTED WHITE
#define COLOR_CHOSEN BLUE
#define COLOR_HEADING BLUE
#define COLOR_ARROW BLACK
#define COLOR_BATTERY BLACK
#define COLOR_BATTERY_LEFT BLACK
#define COLOR_FRAME_BAR WHITE
#define COLOR_LOADING_BAR RED
#define COLOR_COMPLETED_BAR GREEN
#define PULSIOXIMETERCOLOR RED
#define introBackColor WHITE
#define introTextColor BLACK
#define transitionEffect BLACK

int initialSensorPosition = separatorPosition - letter_width;
int pulsioximeterXPos;
char* initialSensorsValue = "XX";
bool controlTemperature;
bool controlHumidity;
int ypos;
bool print_text;
bool display_drawStop = 0;

//hardware verification. 1 is a mounted hardware, 0 a not mounted.
#define hardwareComponents 12
byte errorHardwareCode[hardwareComponents];
#define HWPowerEn 1
#define HWHeater 1
#define HWFAN_HP 1
#define HWFAN_LP 1
#define HWJAUNDICE 1
#define HWSterilize 1
#define HWHumidifier 1
#define HWNTCHeater 1
#define HWNTCRoom 1
#define HWHUMSensor 1

//number assigned to hardware
#define HW_NUM_PowerEn 0         //hardware 1
#define HW_NUM_Heater 1          //hardware 2
#define HW_NUM_FAN_HP 3            //hardware 4
#define HW_NUM_FAN_LP 4            //hardware 5
#define HW_NUM_JAUNDICE 6             //hardware 7
#define HW_NUM_Sterilize 7       //hardware 8
#define HW_NUM_Humidifier 8      //hardware 9
#define HW_NUM_NTCHeater 9       //hardware 10
#define HW_NUM_NTCRoom 10         //hardware 11
#define HW_NUM_HUMSensor 11       //hardware 12


//hardware critical check. 2 is a critical non interchangable hardware, 1 is a critical interchangable hardware, 0 a not critical and interchangable hardware.
#define HW_CRIT_PowerEn 2
#define HW_CRIT_Heater 1
#define HW_CRIT_FAN_HP 0
#define HW_CRIT_FAN_LP 0
#define HW_CRIT_JAUNDICE 1
#define HW_CRIT_Sterilize 1
#define HW_CRIT_Humidifier 1
#define HW_CRIT_NTCHeater 2
#define HW_CRIT_NTCRoom 2
#define HW_CRIT_HUMSENSOR 2

#define shortcircuit 2
#define opencircuit 1

bool hardwareMounted[] = {HWPowerEn, HWHeater, HWFAN_HP, HWFAN_LP, HWJAUNDICE, HWSterilize, HWHumidifier, HWNTCHeater, HWNTCRoom, HWHUMSensor};
bool hardwareCritical[] = {HW_CRIT_PowerEn, HW_CRIT_Heater, HW_CRIT_FAN_HP, HW_CRIT_FAN_LP, HW_CRIT_JAUNDICE, HW_CRIT_Sterilize, HW_CRIT_Humidifier, HW_CRIT_NTCHeater, HW_CRIT_NTCRoom, HW_CRIT_HUMSENSOR};
byte hardwareVerificationPin[] = {POWER_EN, HEATER, FAN_HP, FAN_LP, JAUNDICE, STERILIZE, HUMIDIFIER};
byte hardwareVerificationPin_FB[] = {POWER_EN_FB, HEATER_FB, FAN_HP_FB, FAN_LP_FB, JAUNDICE_FB, STERILIZER_FB, HUMIDIFIER_FB};
char* errorComponent[] = {"Power enable MOSFET", "Heater", "FAN_HP", "FAN_LP", "Jaundice LED", "Sterilizer", "Humidifier", "Temperature sensor", "Temperature sensor", "Humidity sensor"};
bool testOK;
bool testCritical;
bool criticalError;

bool displayProcessPercentage = 0;
int counter;

Adafruit_ILI9341_STM tft = Adafruit_ILI9341_STM(TFT_CS, TFT_DC, TFT_RST); // Use hardware SPI

DHT dht;

//Text Graphic position
int humidityX;
int humidityY;
int temperatureX;
int temperatureY;
int separatorTopYPos;
int separatorBotYPos;
int pulsioximeterYPos;

const byte time_back_draw = 255;
const byte time_back_wait = 255;
const byte fanMaxSpeed = 100;
const byte LEDMaxIntensity = 100;
const byte standardHeaterTempLimit = 85; //max heater temperature
const byte standardFanSpeed = 100;
byte maxHeaterPWM = 255;      //max power for heater, full power (255) is 50W


int page, page0;
bool selected;
int data, instant_read;
byte text_size;
bool pos_text[8];
volatile int EncMove;
long last_ENC_PULSEd;
char* textToWrite;
char* words[8];
char* helpMessage;
byte bar_pos;
byte rectangles;
bool mode;
byte length;
bool manual_state = 0;
long blinking;
long last_something;
bool state_blink;
const int time_blink = 500;
byte missed;
const byte limit_speed = 40; //40
bool int_length, int_length_0;
float factor;
bool ENC_PULSEd, ENC_PULSEd_before;
int time_lock = 16000;
long CheckTempSensorPinTimeout = 45000; //timeout for checking the thermistor pinout
bool auto_lock;
int language;
bool swapTempSensors;
int goBackTextY = tft.height() / 2;
double desiredHeaterTemp;
double desiredRoomTemp = 36.5;
int desiredRoomHum = 80;
double heaterTempLimit;
int fanSpeed;
int LEDIntensity;
long last_temp_update;
long temp_update_rate = 1000;
int backlight_intensity = 100;
bool enableSet;
float temperaturePercentage, temperatureAtStart;
float humidityPercentage, humidityAtStart;
byte temperaturePos, temperature_measured;
int barWidth, barHeight, tempBarPosX, tempBarPosY, humBarPosX, humBarPosY;
byte barThickness;
float diffTemperature[numTempSensors];
int diffHumidity;
bool enableSetProcess;
bool blinkSetMessageState;
int blinkTimeON = 1000;
int blinkTimeOFF = 100;
long lastBlinkSetMessage;
byte goToProcessRow;
volatile long interruptcounter;

// timers
#define sensorsRate 1000    // in microseconds; 
#define NTCInterruptRate 20000    // in microseconds; 
#define roomPIDRate 1000000    // in microseconds; 
#define heaterPIDRate 200000   // times of roomPIDRate;
int roomPIDfactor = roomPIDRate / NTCInterruptRate;
int heaterPIDfactor = heaterPIDRate / NTCInterruptRate;
HardwareTimer sensorsTimer(1);
HardwareTimer roomPIDTimer(2);

void setup() {
  Serial.begin(115200);
  initEEPROM();
  pinDirection();
  initPIDTimers();
  tft.begin();
  tft.setRotation(1);
  //loadLogo();
  dht.setup(DHTPIN);
  /*
    while (1) {
    tft.fillScreen(introTextColor);
    tft.setTextColor(introBackColor);
    hardwareVerification();
    while (digitalRead(ENC_PULSE));
    delay(100);
    while (!digitalRead(ENC_PULSE));
    delay(100);
    }
  */
  Serial.print("IN3ATOR, VERSION ");
  Serial.println(FWversion);
  initSensors();
  EncNewPosition = myEncoderRead();
  EncOldPosition = EncNewPosition;
  menu();
}


void pinDirection() {
  pinMode(SCREENBACKLIGHT, OUTPUT);
  pinMode(ENC_PULSE, INPUT_PULLUP);
  pinMode(JAUNDICE, OUTPUT);
  pinMode(HEATER, OUTPUT);
  pinMode(POWER_EN, OUTPUT);
  pinMode(FAN_HP, OUTPUT);
  pinMode(FAN_LP, OUTPUT);
  pinMode(STERILIZE, OUTPUT);
  pinMode(HUMIDIFIER, OUTPUT);

  digitalWrite(SCREENBACKLIGHT, LOW);
  digitalWrite(JAUNDICE, LOW);
  digitalWrite(HEATER, LOW);
  digitalWrite(POWER_EN, LOW);
  digitalWrite(FAN_HP, LOW);
  digitalWrite(FAN_LP, LOW);
  digitalWrite(STERILIZE, LOW);
  digitalWrite(HUMIDIFIER, LOW);
}
