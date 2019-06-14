#include <Adafruit_GFX_AS.h>
#include <EEPROM.h>
#include <Adafruit_ILI9341_STM.h> // STM32 DMA Hardware-specific library
#include <SPI.h>
#include "DHT.h"
#include <PID_v1.h>
#include "board.h"

#define FWversion "v2.1"
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

//languages
#define firstLanguage 1
#define spanish 1
#define english 2
#define french 3
#define numLanguages 3

Adafruit_ILI9341_STM tft = Adafruit_ILI9341_STM(TFT_CS, TFT_DC, TFT_RST); // Use hardware SPI

DHT dht;

//Text Graphic position
int humidityX;
int humidityY;
int temperatureX;
int temperatureY;

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
long last_pulsed;
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
bool pulsed, pulsed_before;
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
    while (digitalRead(pulse));
    delay(100);
    while (!digitalRead(pulse));
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
  pinMode(pulse, INPUT_PULLUP);
  pinMode(ICT, OUTPUT);
  pinMode(HEATER, OUTPUT);
  pinMode(POWER_EN, OUTPUT);
  pinMode(FAN_HP, OUTPUT);
  pinMode(FAN_LP, OUTPUT);
  pinMode(FAN_EXTRA, OUTPUT);
  pinMode(STERILIZE, OUTPUT);
  pinMode(HUMIDIFIER, OUTPUT);

  digitalWrite(SCREENBACKLIGHT, LOW);
  digitalWrite(ICT, LOW);
  digitalWrite(HEATER, LOW);
  digitalWrite(POWER_EN, LOW);
  digitalWrite(FAN_HP, LOW);
  digitalWrite(FAN_LP, LOW);
  digitalWrite(FAN_EXTRA, LOW);
  digitalWrite(STERILIZE, LOW);
  digitalWrite(HUMIDIFIER, LOW);
}
