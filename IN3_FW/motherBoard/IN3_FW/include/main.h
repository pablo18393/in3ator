#ifndef _MAIN_H
#define _MAIN_H

#include <esp_task_wdt.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
// include libraries
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <EEPROM.h>
#include <SPI.h>
#include <PID_v1.h> //
#include "board.h"
#include <Wire.h>
#include "SparkFun_SHTC3.h"       // Click here to get the library: http://librarymanager/All#SparkFun_SHTC3
#include <RotaryEncoder.h>        //
#include <Beastdevices_INA3221.h> //
#include "in3ator_humidifier.h"
#include "TCA9555.h"
#include "esp32/ulp.h"
#include "driver/rtc_io.h"

#define WDT_TIMEOUT 45

#define ON true
#define OFF false
#define BASIC_CONTROL false
#define PID_CONTROL true
#define SKIN_CONTROL false
#define AIR_CONTROL true

// buzzer variables
#define buzzerStandbyPeriod 10000    // in millis, there will be a periodic tone when regulating baby's constants
#define buzzerStandbyTone 500        // in micros, tone freq
#define buzzerAlarmTone 500          // in micros, tone freq
#define buzzerRotaryEncoderTone 2200 // in micros, tone freq
#define buzzerStandbyToneDuration 50 // in micros, tone freq
#define buzzerSwitchDuration 10      // in micros, tone freq
#define buzzerStandbyToneTimes 1     // in micros, tone freq

// EEPROM variables
#define EEPROM_SIZE 256
#define EEPROM_checkStatus 0
#define EEPROM_firstTurnOn 10
#define EEPROM_autoLock 20
#define EEPROM_language 30
#define EEPROM_SerialNumber 40
#define EEPROM_WIFI_EN 50
#define EEPROM_usedGenericMosfet 60
#define EEPROM_controlMode 70
#define EEPROM_desiredControlMode 80
#define EEPROM_desiredControlHumidity 90
#define EEPROM_RawSkinTemperatureLowCorrection 100
#define EEPROM_RawSkinTemperatureRangeCorrection 110
#define EEPROM_RawAirTemperatureLowCorrection 120
#define EEPROM_RawAirTemperatureRangeCorrection 130
#define EEPROM_RawDigitalTemperatureLowCorrection 140
#define EEPROM_RawDigitalTemperatureRangeCorrection 150
#define EEPROM_controlAlgorithm 160
#define EEPROM_ReferenceTemperatureRange 170
#define EEPROM_ReferenceTemperatureLow 180
#define EEPROM_FineTuneSkinTemperature 190
#define EEPROM_FineTuneAirTemperature 194
#define EEPROM_THINGSBOARD_PROVISIONED 200
#define EEPROM_THINGSBOARD_TOKEN 205

// configuration variables
#define debounceTime 30          // encoder debouncing time
#define timePressToSettings 3000 // in millis, time to press to go to settings window in UI
#define debugUpdatePeriod 1000   // in millis,

// pages number in UI. Configuration and information will be displayed depending on the page number

#define mainMenuPage 1
#define actuatorsProgressPage 2
#define settingsPage 3
#define calibrateSensorsPage 4
#define firstPointCalibrationPage 5
#define secondPointCalibrationPage 6
#define autoCalibrationPage 7
#define fineTuneCalibrationPage 8
// languages numbers that will be called in language variable
#define spanish 0
#define english 1
#define french 2
#define portuguese 3
#define numLanguages 4
#define defaultLanguage english // Preset number configuration when booting for first time

#define secondOrder_filter 3          // amount of temperature samples to filter
#define analog_temperature_filter 500 // amount of temperature samples to filter
#define digital_temperature_filter 10 // amount of temperature samples to filter

#define NTC_MEASUREMENT_PERIOD 1 // in millis
#define NTC_SAMPLES_TEST 1000
#define CURRENT_UPDATE_PERIOD 500 // in millis
#define VOLTAGE_UPDATE_PERIOD 50  // in millis

#define setupAutoCalibrationPoint 0
#define firstAutoCalibrationPoint 1
#define secondAutoCalibrationPoint 2

// GPRS variables to transmit
#define turnedOn 0     // transmit first turned ON with hardware verification
#define room 1         // transmit room variables
#define aliveRefresh 2 // message to let know that incubator is still ON

#define digitalCurrentSensorReadPeriod 500
#define externalADCReadPeriod 30

// sensor variables
#define defaultCurrentSamples 30
#define defaultTestingSamples 8000
#define Rsense 3000 // 3 microohm as shunt resistor
#define digitalCurrentSensor_i2c_address 65
#define roomSensorAddress 112

// #define system constants
#define humidifierDutyCycleMax 100 // maximum humidity cycle in heater to be set
#define humidifierDutyCycleMin 0   // minimum humidity cycle in heater to be set

#define stepTemperatureIncrement 0.1 // maximum allowed temperature to be set
#define stepHumidityIncrement 5      // maximum allowed temperature to be set
#define presetHumidity 60            // preset humidity
#define maxHum 90                    // maximum allowed humidity to be set
#define minHum 20                    // minimum allowed humidity to be set
#define LEDMaxIntensity 100          // max LED intensity to be set
#define fanMaxSpeed 100              // max fan speed (percentage) to be set

// Encoder variables
#define NUMENCODERS 1 // number of encoders in circuit
#define ENCODER_TICKS_DIV 0
#define encPulseDebounce 50

// User Interface display constants
#define brightenRate 10 // intro brighten speed (Higher value, faster)
#define valuePosition 245
#define separatorPosition 240
#define unitPosition 315
#define textFontSize 2 // text default size
#define width_select 7
#define height_heading 34
#define width_indentation 4
#define width_back 50
#define side_gap 4
#define letter_height 26
#define letter_width 14
#define logo 40
#define arrow_height 6
#define arrow_tail 5
#define headint_text_height height_heading / 5
#define initialSensorsValue "XX"
#define barThickness 3
#define blinkTimeON 1000 // displayed text ON time
#define blinkTimeOFF 100 // displayed text OFF time
#define time_back_draw 255
#define time_back_wait 255

#define DEFAULT_AUTOLOCK ON
#define DEFAULT_CONTROL_ALGORITHM PID_CONTROL

typedef enum
{
    HUMIDITY_ALARM = 0,
    TEMPERATURE_ALARM,
    AIR_THERMAL_CUTOUT_ALARM,
    SKIN_THERMAL_CUTOUT_ALARM,
    AIR_SENSOR_ISSUE_ALARM,
    SKIN_SENSOR_ISSUE_ALARM,
    FAN_ISSUE_ALARM,
    HEATER_ISSUE_ALARM,
    NUM_ALARMS,
} ALARMS_ID;

// Graphic variables
#define ERASE false
#define DRAW true

// graphic text configurations
#define graphicTextOffset 1 // bar pos is counted from 1, but text from 0
#define CENTER true
#define LEFT_MARGIN false

// below are all the different variables positions that will be displayed in user interface
// mainMenu
#define controlModeGraphicPosition 0
#define temperatureGraphicPosition 1
#define humidityGraphicPosition 2
#define LEDGraphicPosition 3
#define settingsGraphicPosition 5
#define startGraphicPosition 4
// settings

#define serialNumberGraphicPosition 0
#define languageGraphicPosition 1
#define WifiENGraphicPosition 2
#define calibrateGraphicPosition 3
#define setdefaultValuesGraphicPosition 4
#define HWTestGraphicPosition 5

// calibration menu
#define autoCalibrationGraphicPosition 0
#define fineTuneCalibrationGraphicPosition 1
#define twoPointCalibrationGraphicPosition 2
#define restartCalibrationGraphicPosition 3

// 2p calibration
#define temperatureCalibrationGraphicPosition 0
#define setCalibrationGraphicPosition 1

// auto calibration
#define autoCalibrationMessageGraphicPosition 0

// color options
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
#define COLOR_BAR BLACK
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

#define PIDISRPeriod 1000           // in msecs
#define peripheralsISRPeriod 100000 // in milliseconds

// PID VARIABLES
#define skinPID 0
#define airPID 1
#define humidityPID 2
#define numPID 3

#define BACKLIGHT_NO_INTERACTION_TIME 12000 // time to decrease backlight display if no user actions

#define GPRS_TASK_PERIOD 100
#define OTA_TASK_PERIOD 100
#define SENSORS_TASK_PERIOD 1
#define BUZZER_TASK_PERIOD 10
#define BACKLIGHT_TASK_PERIOD 10
#define BACKLIGHT_DELAY 2
#define INIT_TFT_DELAY 300

typedef struct
{
    double temperature[numSensors];
    double humidity;
    double desiredControlTemperature;
    double desiredControlHumidity;

    double system_current_standby_test;
    double heater_current_test;
    double fan_current_test;
    double phototherapy_current_test;
    double humidifier_current_test;
    double display_current_test;
    double buzzer_current_test;
    double HW_test_error_code;

    double system_current;
    double system_voltage;
    double fan_current;
    double humidifier_current;
    double humidifier_voltage;
    double phototherapy_current;
    int serialNumber = false;

    bool controlMode = AIR_CONTROL;
    bool temperatureControl = false;
    bool humidityControl = false;
    bool phototherapy = false;

    byte language;

} in3ator_parameters;

void logln(String dataString);
long secsToMillis(long timeInMillis);
long minsToMillis(long timeInMillis);
void initHardware(bool printOutputTest);
void UI_mainMenu();
void userInterfaceHandler(int UI_page);
void updateData();
void buzzerHandler();
void buzzerTone(int beepTimes, int timevTaskDelay, int freq);

void shutBuzzer();
double measureMeanConsumption(int shunt);
float measureMeanVoltage(int shunt);
void watchdogReload();
void OTAHandler(void);
void sensorsHandler();
void GPRS_Handler();
void securityCheck();
void buzzerConstantTone(int freq);
void drawAlarmMessage(char *alertMessage);
void drawHeading(int UI_page, int UI_serialNumber);
char *convertStringToChar(String input);
char *convertStringToChar(char *arrayInput, String input);
int16_t drawCentreString(char *string, int16_t dX, int16_t poY, int16_t size);
void eraseBar(int UI_menu_rows, int bar_pos);
void updateBar(int UI_menu_rows, int bar_pos);
void graphics(uint8_t UI_page, uint8_t UI_language, uint8_t UI_print_text, uint8_t UI_menu_rows, uint8_t UI_var_0, uint8_t UI_var_1);
int graphicHeight(int position);
int16_t drawFloat(float floatNumber, int16_t decimal, int16_t poX, int16_t poY, int16_t size);
void setTextColor(int16_t color);

void turnFans(bool mode);
void alarmTimerStart();
void setGPRSPostPeriod(long seconds);

bool ongoingCriticalAlarm();
void setAlarm(byte alarmID);
void resetAlarm(byte alarmID);

void PIDInit();
void PIDHandler();
void startPID(byte var);
void stopPID(byte var);

bool encoderContinuousPress(int UI_page);

void updateLoadingTemperatureBar(float prev, float actual);
void updateLoadingHumidityBar(float prev, float actual);
void drawSelectedTemperature(float temperatureToDraw, float previousTemperatureDrawn);
void drawUnselectedTemperature(float temperatureToDraw, float previousTemperatureDrawn);
void drawHumidity(int UI_humidity, int UI_previousHumdity);
int16_t drawRightString(char *string, int16_t dX, int16_t poY, int16_t size);
void drawStartMessage(bool UI_enableSet, int UI_menu_rows, char *UI_helpMessage);
void drawCentreNumber(int n, int x, int i);
void drawRightNumber(int n, int x, int i);
void drawBack();
void drawActuatorsSeparators();
void drawStop();
void drawHelpMessage(byte UI_language);
void printLoadingTemperatureBar(double UI_desiredControlTemperature);
void printLoadingHumidityBar(int UI_desiredControlHumidity);
void blinkGoBackMessage();
bool ongoingAlarms();
void disableAllAlarms();

void checkSetMessage(int UI_page);
bool checkFan();

bool updateRoomSensor();
void updateDisplaySensors();

void UI_settings();
void UI_actuatorsProgress();

void wifiInit(void);
void wifiDisable();

void loaddefaultValues();
void UI_calibration();
void firstPointCalibration();
void fineTuneCalibration();
void autoCalibration();
void loadDefaultCalibration();
void recapVariables();
void clearCalibrationValues();
void secondPointCalibration();
void saveCalibrationToEEPROM();
int getYpos(int UI_menu_rows, byte row);
bool back_mode();
void setSensorsGraphicPosition(int UI_page);

void basicHumidityControl();
bool checkStableTemperatures(double *referenceSensorHistory, double *sensorToCalibrateHistory, int historyLength, double stabilityError);
void initRoomSensor();
void currentMonitor();
void voltageMonitor();

double butterworth2(double y1, double y2, double x0, double x1, double x2);

void initGPIO();
void initEEPROM();
void initGPRS();
void drawHardwareErrorMessage(long error, bool criticalError);
void watchdogInit();
void initAlarms();
void IRAM_ATTR encSwitchHandler();
void IRAM_ATTR encoderISR();
void IRAM_ATTR ON_OFF_Switch_ISR();
void backlightHandler();

bool measureNTCTemperature(uint8_t);
void loadlogo();

void initPin(uint8_t GPIO, uint8_t Mode);
bool GPIORead(uint8_t GPIO);
void GPIOWrite(uint8_t GPIO, uint8_t Mode);

#endif