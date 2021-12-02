/* * Pending:
    SD
*/

//Firmware version and head title of UI screen
#define FWversion "v6.6"
String serialNumber = "in3000036";
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
#include "TCA9555.h"
#include <Microchip_PAC193x.h>
#include "ESP32TimerInterrupt.h"

#define WDT_TIMEOUT 3


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
int buzzerBeeps, buzzerToneTime;
long buzzerTime, buzzerStandbyTime;
bool buzzerBuzzing;

//Alarm variables
#define powerAlertNotificationPeriod 1000
#define humidityAlarm 0
#define temperatureAlarm 1
#define allAlarms 255
long lastPowerAlertNotification;
bool powerAlert;
bool alarmsEnable;
bool defaultAlarmsEnable = true;
bool alarmOnGoing[2];
int temperatureError = 1; // 1 degrees difference to trigger alarm
int humidityError = 12; //12 %RH to trigger alarm
long temperatureAlarmTime, humidityAlarmTime;
float alarmTimeDelay = 30; //in mins, time to check alarm

//configuration variables
#define debounceTime 10         //encoder debouncing time
#define mosfet_switch_time 100   //delay to wait for mosfet to switch (in millis), oversized
#define timePressToSettings 5000 //in millis, time to press to go to settings window in UI
#define debugUpdatePeriod 1000 //in millis, 
bool UARTDebug = 1;
bool defaultUARTDebug = ON;
long lastDebugUpdate;
long loopCounts;
//pages number in UI. Configuration and information will be displayed depending on the page number
int page;
#define mainMenuPage 1
#define advancedModePage 2
#define actuatorsProgressPage 3
#define settingsPage 4
#define calibrateSensorsPage 5
#define askSuccessPage 6
#define errorPage 7

//languages numbers that will be called in language variable
byte language;
#define spanish 0
#define english 1
#define french 2
#define portuguese 3
#define numLanguages 4
#define defaultLanguage english //Preset number configuration when booting for first time

//number assignment of each enviromental sensor for later call in variable
#define babyNTC 0
#define airNTC 1
#define digitalTempSensor 2

#define numNTC 2 //number of NTC
#define numTempSensors 3 //number of total temperature sensors in system
#define temperature_filter 4000 //amount of temperature samples to filter
#define current_filter 1000 //amount of temperature samples to filter

#define NTCMeasurementPeriod 1000 //in millis
#define CurrentMeasurementPeriod 2 //in millis
#define CurrentUpdatePeriod 1000 //in millis
long lastNTCmeasurement, lastCurrentMeasurement, lastCurrentUpdate;

int NTC_PIN[numNTC] = {BABY_NTC_PIN, AIR_NTC_PIN};
double temperature[numTempSensors];
double temperatureMaxReset = -1000;
double temperatureMinReset = 1000;
double temperatureMax[numTempSensors], temperatureMin[numTempSensors];
double previousTemperature[numTempSensors];
int temperatureArray [numNTC][temperature_filter]; //variable to handle each NTC with the array of last samples (only for NTC)
int temperature_array_pos; //temperature sensor number turn to measure
float diffTemperature[numTempSensors]; //difference between measured temperature and user input real temperature
bool faultNTC[numNTC]; //variable to control a failure in NTC
byte numSensors; //number of total sensors
double humidity; // room humidity variable
int previousHumidity; //previous sampled humidity
float diffHumidity; //difference between measured humidity and user input real humidity

int gestationWeeks = 28; //preterm baby gestation time in weeks
int heaterPower; //duty cycle of control signal of system heater

//Sensor check rate (in ms). Both sensors are checked in same interrupt and they have different check rates
const byte pulsioximeterRate = 5;
byte pulsioximeterCount = false;
byte encoderRate = true;
byte encoderCount = false;
bool encPulseDetected;
volatile long lastEncPulse;
#define encPulseDebounce 10
volatile bool statusEncSwitch;

//WIFI
#define WIFI_connection_try 60000 //in millis
bool WIFI_connection_status = false;

//GPRS variables to transmit
#define turnedOn 0 //transmit first turned ON with hardware verification
#define room 1 //transmit room variables
#define pulsioximeterSensorVariables 2 //transmit heart rate between others
#define pulsioximeterSensorRaw 3 //transmit raw pulsioximeter signal
#define aliveRefresh 4 //message to let know that incubator is still ON

//sensor variables
bool roomSensorPresent;
byte roomSensorAddress = 112;
int pulsioximeterMean;
const int maxPulsioximeterSamples = 320; //(tft width).
int instantCurrent[current_filter];
int instantCurrent_array_pos = false;
float currentConsumption;
float CurrentToAmpFactor = 0.00045;
int currentConsumptionPos = false;
int pulsioximeterSample[maxPulsioximeterSamples][2]; //0 is previous data, 1 is actual data
int pulsioximeterPeakThreshold;
int pulsioximeterMaxPeak;
int pulsioximeterMinPeak;
long pulsioximeterMinPeakTime;
long pulsioximeterMaxPeakTime;
int minPulsioximeterCoordinate;
int maxPulsioximeterCoordinate;
const bool pulsioximeterDrawn = false;
const bool pulsioximeterSampled = true;
int pulsioximeterInterSamples[pulsioximeterRate];
int pulsioximeterMultiplierFilterFactor = 1000;
int pulsioximeterRestFilterFactor = true;
int pulsioximeterAmplitud;
int pulsioximeterCounter[2];
int BPM, IBI;
String RPD; //(Raw Pulsioximeter Data)

//room variables
bool controlMode;
bool defaultcontrolMode = SKIN_CONTROL;
bool controlAlgorithm;
bool defaultcontrolAlgorithm = PID_CONTROL;
double desiredSkinTemp = 34; //preset baby skin temperature
double desiredRoomHum = 75; //preset enviromental humidity
bool jaundiceEnable; //PWM that controls jaundice LED intensity
byte HeaterPower; //maximum heater power
double desiredHeaterTemp; //desired temperature in heater

//preset room variables
const byte defaultHeaterPower = 100; //preset max heater temperature in celsious

//constants
const byte heaterPowerMax = 100; //maximum temperature in heater to be set
const byte heaterPowerMin = false; //maximum temperature in heater to be set
const byte minTemp = 15; //minimum allowed temperature to be set
const byte maxTemp = 45; //maximum allowed temperature to be set
const byte maxHum = 100; //maximum allowed humidity to be set
const byte minHum = 20; //minimum allowed humidity to be set
const byte maxGestation = 99; //maximum gestation weeks to be set
const byte minGestation = true; //minimum gestation weeks to be set
const byte LEDMaxIntensity = 100; //max LED intensity to be set
const byte fanMaxSpeed = 100; //max fan speed (percentage) to be set
const float screenBrightnessFactor = 2.5; //Max brightness will be divided by this constant

//Encoder variables
#define NUMENCODERS 1 //number of encoders in circuit
boolean A_set;
boolean B_set;
int encoderpinA = ENC_A; // pin  encoder A
int encoderpinB = ENC_B; // pin  encoder B
bool encPulsed, encPulsedBefore; //encoder switch status
volatile int EncMove; //moved encoder
volatile int EncMoveOrientation = -1; //set to -1 to increase values clockwise
volatile int last_encoder_move; //moved encoder
long encoder_debounce_time = true; //in milliseconds, debounce time in encoder to filter signal bounces
long last_encPulsed; //last time encoder was pulsed

//User Interface display constants
#define introDelay    1500      //initial delay between intro and menu
#define brightenRate  50        //intro brighten speed (Higher value, slower)
#define valuePosition 245
#define separatorPosition 240
#define unitPosition 315
#define textFontSize 2          //text default size
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
long lastUserInteraction; //last time there was a encoder movement or pulse
long sensorsUpdatePeriod = 4000;
int blinkTimeON = 1000; //displayed text ON time
int blinkTimeOFF = 100; //displayed text OFF time
bool selected;
char* textToWrite;
char* words[8];
char* helpMessage;
byte bar_pos;
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
#define centered 1
#define leftMargin 0

//below are all the different variables positions that will be displayed in user interface
//mainMenu
#define gestationGraphicPosition 0
#define advancedModeGraphicPosition 1
#define LEDGraphicPosition 2
#define startGraphicPosition 3
//Advanced mode
#define controlModeGraphicPosition 0
#define temperatureGraphicPosition 1
#define humidityGraphicPosition 2
#define LEDGraphicPosition 3
#define settingsGraphicPosition 5
#define startGraphicPosition 4
//settings
#define autoLockGraphicPosition 0
#define languageGraphicPosition 1
#define controlAlgorithmGraphicPosition 2
#define heaterPowerGraphicPosition 3
#define DebugENGraphicPosition 4
#define calibrateGraphicPosition 5
#define setdefaultValuesGraphicPosition 6
//calibration
#define airTemperatureCalibrationGraphicPosition 0
#define skinTemperatureCalibrationGraphicPosition 1
#define humidityCalibrationGraphicPosition 2
#define autoCalibrationGraphicPosition 3
#define restartCalibrationGraphicPosition 4
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


Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
SHTC3 mySHTC3;              // Declare an instance of the SHTC3 class
RotaryEncoder encoder(ENC_A, ENC_B, RotaryEncoder::LatchMode::TWO03);
TCA9555 TCA(0x20);
Microchip_PAC193x PAC;
ESP32Timer ITimer0(0);

/*
  HardwareTimer //buzzerTimer(8);
  HardwareTimer //TFTbacklightTimer(4);
  HardwareTimer //PIDTimer(3);
  HardwareTimer //encoderTimer(2);
  HardwareTimer //heaterTimer(1);
*/

#define PIDISRPeriod 1000    // in msecs
#define peripheralsISRPeriod 100000    // in milliseconds

int hardwareComponents;

void setup() {
  initBoard();
  advancedMode();
}

void loop() {

}
