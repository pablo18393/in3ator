
//include libraries
#include <Adafruit_GFX_AS.h>
#include <EEPROM.h>
#include <Adafruit_ILI9341_STM.h> // STM32 DMA Hardware-specific library
#include <SPI.h>
#include "DHT.h"
#include <PID_v1.h>
#include "board.h"
#include "libmaple/dac.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SD.h>
#include <TimeLib.h>


//Firmware version and head title of UI screen
#define FWversion "v2.3"
#define headingTitle "in3ator"
String serialNumber = "bqTest000001";

//configuration variables
#define debounceTime 100         //encoder debouncing time
#define mosfet_switch_time 100   //delay to wait for mosfet to switch (in millis), oversized
#define timePressToSettings 5000 //in millis, time to press to go to settings window in UI

//pages number in UI. Configuration and information will be displayed depending on the page number
int page;
#define mainMenuPage 0
#define advancedModePage 1
#define actuatorsProgressPage 2
#define settingsPage 3
#define calibrateSensorsPage 4
#define askSuccessPage 5

//languages numbers that will be called in language variable
#define defaultLanguage 1 //Preset number configuration when booting for first time
#define spanish 0
#define english 1
#define french 2
#define numLanguages 3
int language;

//temperature variables
#define CheckSensorRaiseTemp 3   //(in celsius degrees) Due to a probable missplacement issue, a Temperature difference threshold is set to distinguish between heater or room NTC at the beggining of a thermal control

//number assignment of each enviromental sensor for later call in variable
#define babyNTC 0
#define heaterNTC 1
#define inBoardLeftNTC 2
#define inBoardRightNTC 3
#define digitalTempSensor 4

#define numNTC 4 //number of out of board NTC
#define numTempSensors 5 //number of total temperature sensors in system
#define temperature_fraction 20 //amount of temperature samples to filter
double temperature[numTempSensors];
double previousTemperature[numTempSensors];
int temperatureArray [numNTC][temperature_fraction]; //variable to handle each NTC with the array of last samples (only for NTC)
byte temperature_measured; //temperature sensor number turn to measure
float diffTemperature[numTempSensors]; //difference between measured temperature and user input real temperature
bool faultNTC[numNTC]; //variable to control a failure in NTC
byte numSensors; //number of total sensors
int humidity; // environmental humidity variable
int previousHumidity; //previous sampled humidity
int diffHumidity; //difference between measured humidity and user input real humidity

int gestationWeeks = 28; //preterm baby gestation time in weeks
int heaterPower; //duty cycle of control signal of system heater

//Sensor check rate (in ms). Both sensors are checked in same interrupt and they have different check rates
const byte pulsioximeterRate = 5;
byte pulsioximeterCount = 0;
byte encoderRate = 1;
byte encoderCount = 0;

//GPRS variables to transmit
#define turnedOn 0 //transmit first turned ON with hardware verification
#define environmental 1 //transmit environmental variables
#define pulsioximeterSensorVariables 2 //transmit heart rate between others
#define pulsioximeterSensorRaw 3 //transmit raw pulsioximeter signal
#define aliveRefresh 4 //message to let know that incubator is still ON


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
int BPM, IBI;
String RPD; //(Raw Pulsioximeter Data)

//environmental variables
double desiredSkinTemp = 36.5; //preset baby skin temperature
int desiredRoomHum = 80; //preset enviromental humidity
int fanSpeed; //PWM that controls fan speed
int jaundiceLEDIntensity; //PWM that controls jaundice LED intensity
double maxHeaterTemp; //maximum heater temperature
double desiredHeaterTemp; //desired temperature in heater

//preset environmental variables
const byte standardmaxHeaterTemp = 85; //preset max heater temperature in celsious
const byte standardFanSpeed = 100;

//constants
const byte heaterMaxTemp = 100; //maximum temperature in heater to be set
const byte minTemp = 15; //minimum allowed temperature to be set
const byte maxTemp = 45; //maximum allowed temperature to be set
const byte maxHum = 100; //maximum allowed humidity to be set
const byte minHum = 20; //minimum allowed humidity to be set
const byte maxGestation = 99; //maximum gestation weeks to be set
const byte minGestation = 1; //minimum gestation weeks to be set
const byte LEDMaxIntensity = 100; //max LED intensity to be set
const byte fanMaxSpeed = 100; //max fan speed (percentage) to be set

//Encoder variables
#define NUMENCODERS 1 //number of encoders in circuit
byte NTCpin[numNTC] = {HEATER_NTC_PIN, BABY_NTC_PIN, INBOARD_LEFT_NTC_PIN, INBOARD_RIGHT_NTC_PIN}; //variable that handles which pin number is heater/room NTC (could be swapped by user in mounting stage)
bool swapTempSensors; //variable to swap room and heater pin map in case are swapped
boolean A_set;
boolean B_set;
int encoderpinA = ENC_A; // pin  encoder A
int encoderpinB = ENC_B; // pin  encoder B
bool encPulsed, encPulsedBefore; //encoder switch status
volatile int EncMove; //moved encoder
long encoder_debounce_time = 1; //in milliseconds, debounce time in encoder to filter signal bounces
long last_encoder_move;
long last_encPulsed; //last time encoder was pulsed

//User Interface display constants
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
#define logo  40
#define arrow_height  6
#define arrow_tail  5

//Text Graphic position variables
int gestationWeeksXPos;
int LEDXPos;
int humidityX;
int humidityY;
int temperatureX;
int temperatureY;
int separatorTopYPos;
int separatorBotYPos;
int pulsioximeterYPos;
int pulsioximeterXPos;
bool controlTemperature;
bool controlHumidity;
int ypos;
bool displayProcessPercentage = 0;
bool print_text;
bool display_drawStop = 0;
char* initialSensorsValue = "XX";
int initialSensorPosition = separatorPosition - letter_width;
byte text_size;
bool pos_text[8];


//Graphic variables
bool enableSet;
float temperaturePercentage, temperatureAtStart;
float humidityPercentage, humidityAtStart;
int barWidth, barHeight, tempBarPosX, tempBarPosY, humBarPosX, humBarPosY;
byte barThickness;

//User Interface display variables
bool auto_lock; //setting that enables backlight switch OFF after a given time of no user actions
int time_lock = 16000; //time to lock screen if no user actions
int TFT_LED_PWR = 210; //PWM that will be supplied to backlight LEDs
const byte time_back_draw = 255;
const byte time_back_wait = 255;
long last_something; //last time there was a encoder movement or pulse
long CheckTempSensorPinTimeout = 45000; //timeout for checking the thermistor pinout
long sensorsUpdateRate = 2000;
int blinkTimeON = 1000; //displayed text ON time
int blinkTimeOFF = 100; //displayed text OFF time
bool selected;
char* textToWrite;
char* words[8];
char* helpMessage;
byte bar_pos;
byte rectangles;
byte length;
long lastSensorsUpdate;
bool enableSetProcess;
long blinking;
bool state_blink;
bool blinkSetMessageState;
long lastBlinkSetMessage;
byte goToProcessRow;

//graphic text configurations
#define graphicTextOffset  1                //bar pos is counted from 1, but text from 0
#define centered 1
#define leftMargin 0

//below are all the different variables positions that will be displayed in user interface
//mainMenu
#define gestationGraphicPosition 0
#define advancedModeGraphicPosition 1
#define LEDGraphicPosition 2
#define startGraphicPosition 3
//Advanced mode
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

//color options
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

//hardware verification variables
#define hardwareComponents 12
byte errorHardwareCode[hardwareComponents];

//hardware verification. 1 is a mounted hardware, 0 a not mounted.
#define HWPowerEn 1
#define HWHeater 1
#define HWFAN_HP 1
#define HWFAN_LP 1
#define HWJAUNDICE 1
#define HWSTERILIZE_CTL 1
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
#define HW_NUM_STERILIZE_CTL 7       //hardware 8
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
#define HW_CRIT_STERILIZE_CTL 1
#define HW_CRIT_Humidifier 1
#define HW_CRIT_NTCHeater 2
#define HW_CRIT_NTCRoom 2
#define HW_CRIT_HUMSENSOR 2

#define shortcircuit 2
#define opencircuit 1

#define ON true
#define OFF false

bool hardwareMounted[] = {HWPowerEn, HWHeater, HWFAN_HP, HWFAN_LP, HWJAUNDICE, HWSTERILIZE_CTL, HWHumidifier, HWNTCHeater, HWNTCRoom, HWHUMSensor};
bool hardwareCritical[] = {HW_CRIT_PowerEn, HW_CRIT_Heater, HW_CRIT_FAN_HP, HW_CRIT_FAN_LP, HW_CRIT_JAUNDICE, HW_CRIT_STERILIZE_CTL, HW_CRIT_Humidifier, HW_CRIT_NTCHeater, HW_CRIT_NTCRoom, HW_CRIT_HUMSENSOR};
byte hardwareVerificationPin[] = {POWER_EN, HEATER, FAN_HP, FAN_LP, JAUNDICE, STERILIZE_CTL, HUMIDIFIER};
byte hardwareVerificationPin_FB[] = {POWER_EN_FB, HEATER_FB, FAN_HP_FB, FAN_LP_FB, JAUNDICE_FB, STERILIZE_FB, HUMIDIFIER_FB};
char* errorComponent[] = {"Power enable MOSFET", "Heater", "FAN_HP", "FAN_LP", "Jaundice LED", "STERILIZE_CTLr", "Humidifier", "Temperature sensor", "Temperature sensor", "Humidity sensor"};
bool testOK;
bool testCritical;
bool criticalError;

Adafruit_ILI9341_STM tft = Adafruit_ILI9341_STM(TFT_CS, TFT_DC, TFT_RST); // Use hardware SPI, tft class definition
DHT dht; //dht sensor class definition
Adafruit_BME280 bme(BME_CS); // hardware SPI, //BME280 (humidity, pressure and temperature sensor) configuration variables

int HeatermaxPWM = maxPWMvalue;      //max power for heater, full power is 50W

// timers configuration
#define sensorsISRRate 5000    // in microseconds, also for BUZZER optimal frequency (2khz); 
#define NTCInterruptRate 20000    // in microseconds; 
#define roomPIDRate 1000000    // in microseconds; 
#define heaterPIDRate 200000   // times of roomPIDRate;
#define GPRSISRRate 1000    // in microseconds, able to read 115200 baud rate uart; 
#define encoderISRRate 1000    // in microseconds; 
#define humidifierTimerRate 9 //in microseconds, to generate a 110Khz PWM for ultra sonic humidifier
int roomPIDfactor = roomPIDRate / NTCInterruptRate;
int heaterPIDfactor = heaterPIDRate / NTCInterruptRate;
volatile long interruptcounter;
/*
  STM32F103RE Timers
  Timer Ch. 1 Ch. 2 Ch. 3 Ch. 4
  1     PA8   PA9   PA10
  2     PA0   PA1   PA2    PA3
  3     PA6   PA7   PB0    PB1
  4     PB6   PB7   PB8    PB9
  8     PC6   PC7   PC8    PC9
*/

HardwareTimer GPRSTimer(1);
HardwareTimer roomPIDTimer(2);
HardwareTimer humidifierTimer(3);
HardwareTimer encoderTimer(4);
HardwareTimer sensorsTimer(8);

void setup() {
  initBoard();
  mainMenu();
}

void loop() {

}
