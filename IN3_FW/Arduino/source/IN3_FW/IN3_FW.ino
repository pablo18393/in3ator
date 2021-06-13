//Firmware version and head title of UI screen
#define FWversion "v5.2"
String serialNumber = "in3000024";
#define headingTitle "in3ator"

#define buzzerStandbyPeriod 10000 //in millis, there will be a periodic tone when regulating baby's constants
#define buzzerStandbyTone 500 //in micros, tone freq
#define buzzerAlarmTone 500 //in micros, tone freq
#define buzzerRotaryEncoderTone 450 //in micros, tone freq
#define buzzerStandbyToneDuration 50 //in micros, tone freq
#define buzzerSwitchDuration 10 //in micros, tone freq
#define buzzerStandbyToneTimes 1 //in micros, tone freq

//include libraries
#include <Adafruit_GFX_AS.h>
#include <EEPROM.h>
#include <Adafruit_ILI9341_STM.h> // STM32 DMA Hardware-specific library
#include <SPI.h>put
#include <PID_v1.h>
#include "board.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <SD.h>
#include <TimeLib.h>
#include "SparkFun_SHTC3.h" // Click here to get the library: http://librarymanager/All#SparkFun_SHTC3
#include <RotaryEncoder.h>

#define LIBMAPPLE_CORE //comment it for HAL based core

#define IWDG_PR_DIV_4 0x0
#define IWDG_PR_DIV_8 0x1
#define IWDG_PR_DIV_16 0x2
#define IWDG_PR_DIV_32 0x3
#define IWDG_PR_DIV_64 0x4
#define IWDG_PR_DIV_128 0x5
#define IWDG_PR_DIV_256 0x6

typedef enum iwdg_prescaler {
  IWDG_PRE_4 = IWDG_PR_DIV_4,     /**< Divide by 4 */
  IWDG_PRE_8 = IWDG_PR_DIV_8,     /**< Divide by 8 */
  IWDG_PRE_16 = IWDG_PR_DIV_16,   /**< Divide by 16 */
  IWDG_PRE_32 = IWDG_PR_DIV_32,   /**< Divide by 32 */
  IWDG_PRE_64 = IWDG_PR_DIV_64,   /**< Divide by 64 */
  IWDG_PRE_128 = IWDG_PR_DIV_128, /**< Divide by 128 */
  IWDG_PRE_256 = IWDG_PR_DIV_256  /**< Divide by 256 */
} iwdg_prescaler;

#if defined(LIBMAPPLE_CORE)
typedef struct iwdg_reg_map {
  volatile uint32_t KR;  /**< Key register. */
  volatile uint32_t PR;  /**< Prescaler register. */
  volatile uint32_t RLR; /**< Reload register. */
  volatile uint32_t SR;  /**< Status register */
} iwdg_reg_map;

#define IWDG ((struct iwdg_reg_map *)0x40003000)
#endif

#define ON true
#define OFF false
#define BASIC_CONTROL false
#define PID_CONTROL true

//Alarm variables
#define powerAlertNotificationPeriod 1000
long lastPowerAlertNotification;
bool powerAlert;
bool alarmsEnable;
bool defaultAlarmsEnable = true;
int temperatureError = 2; // 2 degrees in
int humidityError = 12; //12 %RH to trigger alarm
long temperatureAlarmTime, humidityAlarmTime;
int alarmTimeDelay = 1; //in mins, time to check alarm

//configuration variables
#define debounceTime 10         //encoder debouncing time
#define mosfet_switch_time 100   //delay to wait for mosfet to switch (in millis), oversized
#define timePressToSettings 5000 //in millis, time to press to go to settings window in UI
#define debugUpdatePeriod 1000 //in millis, 
bool UARTDebug;
bool defaultUARTDebug = OFF;
long lastDebugUpdate;
//pages number in UI. Configuration and information will be displayed depending on the page number
int page;
#define mainMenuPage 0
#define advancedModePage 1
#define actuatorsProgressPage 2
#define settingsPage 3
#define calibrateSensorsPage 4
#define askSuccessPage 5
#define HWErrorPage 6

//languages numbers that will be called in language variable
int language;
#define spanish 0
#define english 1
#define french 2
#define portuguese 3
#define numLanguages 4
#define defaultLanguage english //Preset number configuration when booting for first time

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
#define temperature_fraction 50 //amount of temperature samples to filter
double temperature[numTempSensors];
double previousTemperature[numTempSensors];
int temperatureArray [numNTC][temperature_fraction]; //variable to handle each NTC with the array of last samples (only for NTC)
byte temperature_measured; //temperature sensor number turn to measure
float diffTemperature[numTempSensors]; //difference between measured temperature and user input real temperature
bool faultNTC[numNTC]; //variable to control a failure in NTC
byte numSensors; //number of total sensors
int humidity; // room humidity variable
int previousHumidity; //previous sampled humidity
int diffHumidity; //difference between measured humidity and user input real humidity

int gestationWeeks = 28; //preterm baby gestation time in weeks
int heaterPower; //duty cycle of control signal of system heater

//Sensor check rate (in ms). Both sensors are checked in same interrupt and they have different check rates
const byte pulsioximeterRate = 5;
byte pulsioximeterCount = 0;
byte encoderRate = 1;
byte encoderCount = 0;
bool encPulseDetected;

//GPRS variables to transmit
#define turnedOn 0 //transmit first turned ON with hardware verification
#define room 1 //transmit room variables
#define pulsioximeterSensorVariables 2 //transmit heart rate between others
#define pulsioximeterSensorRaw 3 //transmit raw pulsioximeter signal
#define aliveRefresh 4 //message to let know that incubator is still ON

//sensor variables
byte roomSensorPresent;
byte roomSensorAddress = 112;
int pulsioximeterMean;
const int maxPulsioximeterSamples = 320; //(tft width).
float currentConsumption, currentConsumtionStacker;
float currentOffset = 110;
float correctionCurrentFactor = 0.0038;
int currentConsumptionPos = 0;
float currentConsumptionFactor = 2.685; //factor to translate current consumtion in mA
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

//room variables
bool controlMode;
bool defaultControlMode = BASIC_CONTROL;
double desiredSkinTemp = 34; //preset baby skin temperature
int desiredRoomHum = 75; //preset enviromental humidity
bool jaundiceEnable; //PWM that controls jaundice LED intensity
double HeaterPower; //maximum heater power
double desiredHeaterTemp; //desired temperature in heater

//preset room variables
const byte defaultHeaterPower = 100; //preset max heater temperature in celsious

//constants
const byte heaterPowerMax = 100; //maximum temperature in heater to be set
const byte heaterPowerMin = 10; //maximum temperature in heater to be set
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
boolean A_set;
boolean B_set;
int encoderpinA = ENC_A; // pin  encoder A
int encoderpinB = ENC_B; // pin  encoder B
bool encPulsed, encPulsedBefore; //encoder switch status
volatile int EncMove; //moved encoder
int EncMoveOrientation = -1; //set to -1 to increase values clockwise
volatile int last_encoder_move; //moved encoder
long encoder_debounce_time = 1; //in milliseconds, debounce time in encoder to filter signal bounces
long last_encPulsed; //last time encoder was pulsed

//User Interface display constants
#define introDelay    1500      //initial delay between intro and menu
#define brightenRate  100        //intro brighten speed (Higher value, slower)
#define valuePosition 245
#define separatorPosition 240
#define unitPosition 315
#define textFontSize 4          //text default size
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
bool autoLock; //setting that enables backlight switch OFF after a given time of no user actions
bool defaultAutoLock = OFF; //setting that enables backlight switch OFF after a given time of no user actions
int time_lock = 16000; //time to lock screen if no user actions
int TFT_LED_PWR = 25000; //PWM that will be supplied to backlight LEDs
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
#define temperatureGraphicPosition 0
#define humidityGraphicPosition 1
#define LEDGraphicPosition 2
#define settingsGraphicPosition 4
#define startGraphicPosition 3
//settings
#define autoLockGraphicPosition 0
#define languageGraphicPosition 1
#define controlModeGraphicPosition 2
#define heaterPowerGraphicPosition 3
#define DebugENGraphicPosition 4
#define calibrateGraphicPosition 5
#define setdefaultValuesGraphicPosition 6
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

Adafruit_ILI9341_STM tft = Adafruit_ILI9341_STM(TFT_CS, TFT_DC, TFT_RST); // Use hardware SPI, tft class definition
SHTC3 mySHTC3;              // Declare an instance of the SHTC3 class
TwoWire WIRE2 (2, I2C_FAST_MODE);
#define Wire WIRE2
RotaryEncoder encoder(ENC_A, ENC_B, RotaryEncoder::LatchMode::TWO03);

// timers configuration
/*
  STM32F103RE Timers
  Timer Ch. 1 Ch. 2 Ch. 3 Ch. 4
  1     PA8   PA9   PA10
  2     PA0   PA1   PA2    PA3
  3     PA6   PA7   PB0    PB1
  4     PB6   PB7   PB8    PB9
  8     PC6   PC7   PC8    PC9
*/

HardwareTimer buzzerTimer(8);
HardwareTimer TFTbacklightTimer(4);
HardwareTimer PIDTimer(3);
HardwareTimer encoderTimer(2);
HardwareTimer heaterTimer(1);

#define buzzerdefaultPeriod 500    // in microseconds, also for BUZZER optimal frequency (2khz); Prescale factor 6, Overflow 36000
#define PIDISRPeriod 200000    // in microseconds
#define peripheralsISRPeriod 1000    // in microseconds
#define heaterTimerPeriod 20*peripheralsISRPeriod   // in microseconds. Heater period, antialiasing factor for current sensing
#define backlightTimerPeriod 100 //in microseconds


int hardwareComponents;

void setup() {
  initBoard();
  mainMenu();
}

void loop() {

}
