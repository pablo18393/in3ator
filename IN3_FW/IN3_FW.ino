#include <Adafruit_GFX_AS.h>
#include <EEPROM.h>
#include <Adafruit_ILI9341_STM.h> // STM32 DMA Hardware-specific library
#include <SPI.h>
#include "DHT.h"
#include <PID_v1.h>

#define FWversion "v1.10"
#define headingTitle "in3ator"

//configuration variables
#define debounceTime 100        //encoder debouncing time
#define maxPWMvalue 255         //for maple mini
#define maxHeaterPWM 255        //max power for heater, full power (255) is 50W
#define temperature_fraction 20 //times to measure in a 
#define mosfet_switch_time 100   //in millis, oversized
#define timePressToSettings 5000 //in millis, time to press to go to settings window
#define CheckSensorRaiseTemp 3   //in celsius degrees, the amount of degrees to differentiate heater or room NTC
#define roomNTC 0
#define heaterNTC 1
#define bothNTC 2
#define dhtSensor 2
#define numNTC 2
#define numTempSensors 3

//EEPROM VARIABLES
#define EEPROM_firstTurnOn 0
#define EEPROM_autoLock 1
#define EEPROM_language 2
#define EEPROM_heaterLimitTemp 3
#define EEPROM_diffHumidity 10
#define EEPROM_diffTemperature 20
#define EEPROM_swapTempSensors 30
#define EEPROM_usedGenericMosfet 40
#define EEPROM_checkStatus 100

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
int initialSensorPosition = separatorPosition - letter_width;
char* initialSensorsValue = "XX";
bool controlTemperature;
bool controlHumidity;
int ypos;
bool print_text;

//pages
#define menuPage 0
#define actuatorsProgressPage 1
#define settingsPage 2
#define calibrateSensorsPage 3

//languages
#define firstLanguage 1
#define spanish 1
#define english 2
#define french 3
#define numLanguages 3

//graphic positions
#define graphicTextOffset 1                //bar pos is counted from 1, but text from 0
//menu
#define temperatureGraphicPosition 0
#define humidityGraphicPosition 1
#define LEDGraphicPosition 2
#define settingsGraphicPosition 4
#define startGraphicPosition 3
//settings
#define autoLockGraphicPosition 0
#define languageGraphicPosition 1
#define heaterTempGraphicPosition 2
#define setStandardValuesGraphicPosition 3
#define calibrateGraphicPosition 4
//settings
#define temperatureCalibrationGraphicPosition 0
#define humidityCalibrationGraphicPosition 1
#define restartCalibrationValuesTempGraphicPosition 2

//pin declaration
//boardPWMPins: 3, 4, 5, 8, 9, 10, 11, 15, 16, 25, 26, 27

// Use hardware SPI lines+
//Pins from 4-6 are SPI
#define TFT_CS         7
#define TFT_DC         31
#define TFT_RST        2

#define DHTPIN 0
#define SCREENBACKLIGHT 3
byte THERMISTOR_HEATER = 10;
byte THERMISTOR_ROOM = 11;
#define ENCODER_A 12
#define ENCODER_B 13
#define POWER_EN 18
#define FAN1 9
#define FAN2 8
#define FAN3 15
#define HEATER 16
#define GENERIC 16
#define ICT 25
#define STERILIZE 28
#define HUMIDIFIER 27
#define HEATER_FB 17
#define POWER_EN_FB 19
#define FAN1_FB 20
#define FAN2_FB 21
#define FAN3_FB 22
#define ICT_FB 26
#define STERILIZE_FB 29
#define HUMIDIFIER_FB 30
#define pulse 14

//hardware verification. 1 is a mounted hardware, 0 a not mounted.
#define hardwareComponents 12
byte errorHardwareCode[hardwareComponents];
#define HWPowerEn 0
#define HWHeater 1
#define HWGeneric 1
#define HWFan1 1
#define HWFan2 1
#define HWFan3 0
#define HWICT 1
#define HWSterilize 0
#define HWHumidifier 1
#define HWNTCHeater 1
#define HWNTCRoom 1
#define HWHUMSENSOR 1

//number assigned to hardware
#define HW_NUM_PowerEn 1         //hardware 1
#define HW_NUM_Heater 2          //hardware 2
#define HW_NUM_Generic 3         //hardware 3
#define HW_NUM_Fan1 4            //hardware 4
#define HW_NUM_Fan2 5            //hardware 5
#define HW_NUM_Fan3 6            //hardware 6
#define HW_NUM_ICT 7             //hardware 7
#define HW_NUM_Sterilize 8       //hardware 8
#define HW_NUM_Humidifier 9      //hardware 9
#define HW_NUM_NTCHeater 10       //hardware 10
#define HW_NUM_NTCRoom 11         //hardware 11
#define HW_NUM_HUMSENSOR 12       //hardware 12


//hardware critical check. 2 is a critical non interchangable hardware, 1 is a critical interchangable hardware, 0 a not critical and interchangable hardware.
#define HW_CRIT_PowerEn 2
#define HW_CRIT_Heater 1
#define HW_CRIT_Generic 0
#define HW_CRIT_Fan1 0
#define HW_CRIT_Fan2 0
#define HW_CRIT_Fan3 0
#define HW_CRIT_ICT 1
#define HW_CRIT_Sterilize 1
#define HW_CRIT_Humidifier 1
#define HW_CRIT_NTCHeater 2
#define HW_CRIT_NTCRoom 2
#define HW_CRIT_HUMSENSOR 2

bool hardwareMounted[] = {HWPowerEn, HWHeater, HWGeneric, HWFan1, HWFan2, HWFan3, HWICT, HWSterilize, HWHumidifier, HWNTCHeater, HWNTCRoom};
bool hardwareCritical[] = {HW_CRIT_PowerEn, HW_CRIT_Heater, HW_CRIT_Generic, HW_CRIT_Fan1, HW_CRIT_Fan2, HW_CRIT_Fan3, HW_CRIT_ICT, HW_CRIT_Sterilize, HW_CRIT_Humidifier, HW_CRIT_NTCHeater, HW_CRIT_NTCRoom};
byte hardwareVerificationPin[] = {POWER_EN, HEATER, GENERIC, FAN1, FAN2, FAN3, ICT, STERILIZE, HUMIDIFIER, THERMISTOR_HEATER, THERMISTOR_ROOM};
#define errorComponent[]={"Power enable MOSFET","Heater","Generic","Fan1","Fan2","Fan3","Jaundice LED","Sterilizer","Humidifier","Temperature sensor","Temperature sensor"}
#define shortcircuit 2
#define opencircuit 1
bool testOK;
bool testCritical;


Adafruit_ILI9341_STM tft = Adafruit_ILI9341_STM(TFT_CS, TFT_DC, TFT_RST); // Use hardware SPI

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
#define introBackColor WHITE
#define introTextColor BLACK
#define transitionEffect BLACK

DHT dht;

#define MAXENCODERS 1
byte NTCpin[numNTC] = {THERMISTOR_HEATER, THERMISTOR_ROOM};
volatile int encstate[MAXENCODERS];
volatile int encflag[MAXENCODERS];
boolean A_set[MAXENCODERS];
boolean B_set[MAXENCODERS];
volatile int16_t encoderpos[MAXENCODERS];
volatile int  encodertimer = millis(); // acceleration measurement
int encoderpinA[MAXENCODERS] = {ENCODER_A}; // pin array of all encoder A inputs
int encoderpinB[MAXENCODERS] = {ENCODER_B}; // pin array of all encoder B inputs
unsigned int lastEncoderPos[MAXENCODERS];

//Text position
int humidityX;
int humidityY;
int temperatureX;
int temperatureY;

//constants
const byte heaterMaxTemp = 100;
const byte fanMaxSpeed = 100;
const byte LEDMaxIntensity = 100;
const byte time_back_draw = 255;
const byte time_back_wait = 255;
const byte minTemp = 15;
const byte maxTemp = 45;
const byte maxHum = 100;
const byte minHum = 20;

bool criticalError;
int page, page0;
bool selected;
int data, instant_read;
byte text_size;
bool pos_text[8];
volatile int move;
long last_pulsed;
char* textToWrite;
char* words[8];
char* helpMessage;
byte bar_pos;
int oldPosition;
int newPosition;
byte rectangles;
bool manualMode;
bool mode;
byte xpos;
byte length;
bool manual_state = 0;
long blinking;
long last_something;
bool state_blink;
const int time_blink = 500;
byte missed;
int speed;
const byte limit_speed = 40; //40
bool moved;
bool int_length, int_length_0;
byte next;
float factor;
bool pulsed, pulsed_before;
int time_lock = 16000;
long CheckTempSensorPinTimeout = 45000; //timeout for checking the thermistor pinout
bool auto_lock;
byte counter;
int language;
bool swapTempSensors;
int goBackTextY = tft.height() / 2;

double temperature[numTempSensors];
double previousTemperature[numTempSensors];
int previousHumidity;
double desiredHeaterTemp;
int humidity;
double desiredRoomTemp = 36.5;
int desiredRoomHum = 80;
double heaterLimitTemp;
int fanSpeed;
int LEDIntensity;
long last_temp_update;
long temp_update_rate = 1000;
int backlight_intensity = 100;
bool enableSet;
float temperaturePercentage, temperatureAtStart;
float humidityPercentage, humidityAtStart;
int temperatureArray [numNTC][temperature_fraction];
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

//PID VARIABLES
double Kp_heater = 0.1, Ki_heater = 0.3, Kd_heater = 0.1;
double Kp_room = 0.1, Ki_room = 0.3, Kd_room = 0.1;
double PIDOutput[2];
bool temperaturePIDcontrolStart;
PID heaterPID(&temperature[heaterNTC], &PIDOutput[heaterNTC], &desiredHeaterTemp, Kp_heater, Ki_heater, Kd_heater, P_ON_M, DIRECT);
PID roomPID(&temperature[roomNTC], &PIDOutput[roomNTC], &desiredRoomTemp, Kp_room, Ki_room, Kd_room, P_ON_M, DIRECT);
// timer
#define temperaturePIDcontrol 0         //0 to disable, 1 to enable
#define ENCODER_RATE 1000    // in microseconds; 
#define NTCInterruptRate 20000    // in microseconds; 
#define roomPIDRate 1000000    // in microseconds; 
#define heaterPIDRate 200000   // times of roomPIDRate;
int roomPIDfactor = roomPIDRate / NTCInterruptRate;
int heaterPIDfactor = heaterPIDRate / NTCInterruptRate;
HardwareTimer timer(1);
HardwareTimer roomPIDTimer(2);

void setup() {
  Serial.begin(115200);
  initEEPROM();
  pinDirection();
  initPIDTimers();
  tft.begin();
  tft.setRotation(1);
  loadLogo();
  dht.setup(DHTPIN);
  //hardwareVerification();
  initEncoders();
  newPosition = myEncoderRead();
  oldPosition = newPosition;
  menu();
}


void pinDirection() {
  pinMode(SCREENBACKLIGHT, OUTPUT);
  pinMode(pulse, INPUT_PULLUP);
  pinMode(ICT, OUTPUT);
  pinMode(HEATER, OUTPUT);
  pinMode(POWER_EN, OUTPUT);
  pinMode(FAN1, OUTPUT);
  pinMode(FAN2, OUTPUT);
  pinMode(FAN3, OUTPUT);
  pinMode(STERILIZE, OUTPUT);
  pinMode(HUMIDIFIER, OUTPUT);
}
