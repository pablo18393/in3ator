/* * Pending:
    SD
*/

//Firmware version and head title of UI screen
#define FWversion "v9.11/8.B"
#define headingTitle "in3ator"

#include <esp_task_wdt.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
//include libraries
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <EEPROM.h>
#include <SPI.h>
#include <PID_v1.h>
#include "board.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
//#include <SD.h>
#include "SparkFun_SHTC3.h" // Click here to get the library: http://librarymanager/All#SparkFun_SHTC3
#include <RotaryEncoder.h>
#include <Microchip_PAC193x.h>
#include <SparkFun_ADS122C04_ADC_Arduino_Library.h> // Click here to get the library: http://librarymanager/All#SparkFun_ADS122C0


TwoWire *wire;
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
SHTC3 mySHTC3;              // Declare an instance of the SHTC3 class
RotaryEncoder encoder(ENC_A, ENC_B, RotaryEncoder::LatchMode::TWO03);
#define Rsense 3000 //3 microohm as shunt resistor
Microchip_PAC193x PAC(Rsense);
SFE_ADS122C04 mySensor;
#define externalADC_i2c_address 64
#define digitalCurrentSensor_i2c_address 57
#define roomSensorAddress 112


int serialNumber = 45;
#define WDT_TIMEOUT 15

byte HWAlternativePinout;

#define ON true
#define OFF false
#define BASIC_CONTROL false
#define PID_CONTROL true
#define SKIN_CONTROL false
#define AIR_CONTROL true

//buzzer variables
#define buzzerStandbyPeriod 10000 //in millis, there will be a periodic tone when regulating baby's constants
#define buzzerStandbyTone 500 //in micros, tone freq
#define buzzerAlarmTone 500 //in micros, tone freq
#define buzzerRotaryEncoderTone 2200 //in micros, tone freq
#define buzzerStandbyToneDuration 50 //in micros, tone freq
#define buzzerSwitchDuration 10 //in micros, tone freq
#define buzzerStandbyToneTimes 1 //in micros, tone freq


//configuration variables
#define debounceTime 30         //encoder debouncing time
#define timePressToSettings 3000 //in millis, time to press to go to settings window in UI
#define debugUpdatePeriod 1000 //in millis, 
bool WIFI_EN = true;
bool defaultWIFI_EN = ON;
long lastDebugUpdate;
long loopCounts;
//pages number in UI. Configuration and information will be displayed depending on the page number
int page;
#define mainMenuPage 1
#define actuatorsProgressPage 2
#define settingsPage 3
#define calibrateSensorsPage 4
#define errorPage 5
#define firstPointCalibrationPage 6
#define secondPointCalibrationPage 7
#define autoCalibrationPage 8
#define fineTuneCalibrationPage 9
//languages numbers that will be called in language variable
byte language;
#define spanish 0
#define english 1
#define french 2
#define portuguese 3
#define numLanguages 4
#define defaultLanguage english //Preset number configuration when booting for first time

//number assignment of each enviromental sensor for later call in variable
#define skinSensor 0
#define airSensor 1
#define numNTC 1 //number of NTC
#define numSensors 2 //number of total temperature sensors in system

#define secondOrder_filter 3 //amount of temperature samples to filter
#define analog_temperature_filter 500 //amount of temperature samples to filter
#define digital_temperature_filter 10 //amount of temperature samples to filter
int temperature_filter = analog_temperature_filter; //amount of temperature samples to filter

#define NTCMeasurementPeriod 1000 //in millis
#define CurrentMeasurementPeriod 500 //in micros
#define CurrentUpdatePeriod 1000 //in millis
long lastNTCmeasurement, lastCurrentMeasurement, lastCurrentUpdate;

int NTC_PIN[numNTC] = {BABY_NTC_PIN};
double temperature[numSensors];
double errorTemperature[numSensors], temperatureCalibrationPoint;
double ReferenceTemperatureRange, ReferenceTemperatureLow;
double provisionalReferenceTemperatureLow;
double fineTuneSkinTemperature;
double RawTemperatureLow[numSensors], RawTemperatureRange[numSensors];
double provisionalRawTemperatureLow[numSensors];
double temperatureMaxReset = -1000;
double temperatureMinReset = 1000;
double temperatureMax[numSensors], temperatureMin[numSensors];
int temperatureArray [numNTC][analog_temperature_filter]; //variable to handle each NTC with the array of last samples (only for NTC)
int temperature_array_pos; //temperature sensor number turn to measure
float diffTemperature; //difference between measured temperature and user input real temperature
bool faultNTC[numNTC]; //variable to control a failure in NTC
double humidity; // room humidity variable
bool humidifierState, humidifierStateChange;
int previousHumidity; //previous sampled humidity
float diffHumidity; //difference between measured humidity and user input real humidity


byte autoCalibrationProcess;
#define setupAutoCalibrationPoint 0
#define firstAutoCalibrationPoint 1
#define secondAutoCalibrationPoint 2

//Sensor check rate (in ms). Both sensors are checked in same interrupt and they have different check rates
byte encoderRate = true;
byte encoderCount = false;
bool encPulseDetected;
volatile long lastEncPulse;
#define encPulseDebounce 50
volatile bool statusEncSwitch;

//WIFI
#define WIFI_connection_try 60000 //in millis
bool WIFI_connection_status = false;

//GPRS variables to transmit
#define turnedOn 0 //transmit first turned ON with hardware verification
#define room 1 //transmit room variables
#define aliveRefresh 2 //message to let know that incubator is still ON

#define digitalCurrentSensorReadPeriod 500
#define externalADCReadPeriod 30

//sensor variables
#define defaultCurrentSamples 30
#define defaultTestingSamples 8000

bool roomSensorPresent = false;
bool externalADCpresent = false;
bool digitalCurrentSensorPresent = false;
long lastDigitalCurrentSensorRead;
long lastexternalADCRead;
float instantCurrent[currentSensingNum][secondOrder_filter];
float previousCurrent[currentSensingNum][secondOrder_filter];

float instantTemperature[secondOrder_filter];
float previousTemperature[secondOrder_filter];

int instantCurrent_array_pos = false;
float currentConsumption[currentSensingNum];
float currentToAmpFactor_MAIN = 0.003;
float currentToAmpFactor_HUMIDIFIER = 0.00013;

//room variables
bool controlMode;
bool defaultcontrolMode = SKIN_CONTROL;
bool controlAlgorithm;
bool defaultcontrolAlgorithm = PID_CONTROL;
double desiredControlTemperature; //preset baby skin temperature
double desiredControlHumidity; //preset enviromental humidity
bool jaundiceEnable; //PWM that controls jaundice LED intensity
double desiredHeaterTemp; //desired temperature in heater

//constants
const byte humidifierDutyCycleMax = 100; //maximum humidity cycle in heater to be set
const byte humidifierDutyCycleMin = 0; //minimum humidity cycle in heater to be set
const float minDesiredTemp[2] = {35, 30}; //minimum allowed temperature to be set
const float maxDesiredTemp[2] = {37.5, 37}; //maximum allowed temperature to be set
const float stepTemperatureIncrement = 0.1; //maximum allowed temperature to be set
const float stepHumidityIncrement = 5; //maximum allowed temperature to be set
const int presetTemp[2] = {36, 32}; //preset baby skin temperature
const int presetHumidity = 60; //preset humidity
const byte maxHum = 90; //maximum allowed humidity to be set
const byte minHum = 20; //minimum allowed humidity to be set
const byte LEDMaxIntensity = 100; //max LED intensity to be set
const byte fanMaxSpeed = 100; //max fan speed (percentage) to be set
const float screenBrightnessFactor = 2.5; //Max brightness will be divided by this constant

//Encoder variables
#define NUMENCODERS 1 //number of encoders in circuit
#define ENCODER_TICKS_DIV 0
boolean A_set;
boolean B_set;
int encoderpinA = ENC_A; // pin  encoder A
int encoderpinB = ENC_B; // pin  encoder B
bool encPulsed, encPulsedBefore; //encoder switch status
bool updateUIData;
volatile int EncMove; //moved encoder
volatile int lastEncMove; //moved last encoder
volatile int EncMoveOrientation = -1; //set to -1 to increase values clockwise
volatile int last_encoder_move; //moved encoder
long encoder_debounce_time = true; //in milliseconds, debounce time in encoder to filter signal bounces
long last_encPulsed; //last time encoder was pulsed

//User Interface display constants
#define brightenRate  10        //intro brighten speed (Higher value, faster)
#define valuePosition 245
#define separatorPosition 240
#define unitPosition 315
#define textFontSize 2          //text default size
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
#define headint_text_height height_heading / 5

//Text Graphic position variables
int LEDXPos;
int humidityX;
int humidityY;
int temperatureX;
int temperatureY;
int separatorTopYPos;
int separatorBotYPos;
bool controlTemperature;
bool controlHumidity;
int ypos;
bool displayProcessPercentage = false;
bool print_text;
bool display_drawStop = false;
char initialSensorsValue[] = "XX";
int initialSensorPosition = separatorPosition - letter_width;
byte text_size;
bool pos_text[8];


//Graphic variables
#define ERASE false
#define DRAW  true
bool enableSet;
float temperaturePercentage, temperatureAtStart;
float humidityPercentage, humidityAtStart;
int barWidth, barHeight, tempBarPosX, tempBarPosY, humBarPosX, humBarPosY;
byte barThickness;
int screenTextColor, screenTextBackgroundColor;

//User Interface display variables
bool autoLock; //setting that enables backlight switch OFF after a given time of no user actions
bool defaultAutoLock = ON; //setting that enables backlight switch OFF after a given time of no user actions
int time_lock = 16000; //time to lock screen if no user actions
const byte time_back_draw = 255;
const byte time_back_wait = 255;
long lastbacklightHandler; //last time there was a encoder movement or pulse
long sensorsUpdatePeriod = 1000;
int blinkTimeON = 1000; //displayed text ON time
int blinkTimeOFF = 100; //displayed text OFF time
bool selected;
char cstring[100];
char* textToWrite;
char* words[12];
char* helpMessage;
byte bar_pos = true;
byte rectangles;
byte length;
long lastGraphicSensorsUpdate;
long lastSensorsUpdate;
bool enableSetProcess;
long blinking;
bool state_blink;
bool blinkSetMessageState;
long lastBlinkSetMessage;
byte goToProcessRow;

//graphic text configurations
#define graphicTextOffset  1                //bar pos is counted from 1, but text from 0
#define CENTER true
#define LEFT_MARGIN false

//below are all the different variables positions that will be displayed in user interface
//mainMenu
#define controlModeGraphicPosition 0
#define temperatureGraphicPosition 1
#define humidityGraphicPosition 2
#define LEDGraphicPosition 3
#define settingsGraphicPosition 5
#define startGraphicPosition 4
//settings

#define serialNumberGraphicPosition 0
#define languageGraphicPosition 1
#define WifiENGraphicPosition 2
#define calibrateGraphicPosition 3
#define setdefaultValuesGraphicPosition 4
#define HWTestGraphicPosition 5
//old settings
#define autoLockGraphicPosition 6
#define controlAlgorithmGraphicPosition 7

//calibration menu
#define autoCalibrationGraphicPosition 0
#define fineTuneCalibrationGraphicPosition 1
#define twoPointCalibrationGraphicPosition 2
#define restartCalibrationGraphicPosition 3

//2p calibration
#define temperatureCalibrationGraphicPosition 0
#define setCalibrationGraphicPosition 1

//auto calibration
#define autoCalibrationMessageGraphicPosition 0

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
#define introBackColor WHITE
#define introTextColor BLACK
#define transitionEffect BLACK

#define PIDISRPeriod 1000    // in msecs
#define peripheralsISRPeriod 100000    // in milliseconds

int hardwareComponents;

void setup() {
  initHardware(false);
  UI_mainMenu();
}

void loop() {
  userInterfaceHandler();
  updateData();
}
