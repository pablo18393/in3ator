
/*page:
   0 menu
   1 processPage
   2 settings
   3 calibrate sensors
*/
#include <Adafruit_GFX_AS.h>
#include <EEPROM.h>
#include <Adafruit_ILI9341_STM.h> // STM32 DMA Hardware-specific library
#include <SPI.h>
#include "DHT.h"
#include <PID_v1.h>

//EEPROM VARIABLES
#define EEPROM_firstTurnOn 0
#define EEPROM_autoLock 1
#define EEPROM_language 2
#define EEPROM_heaterLimitTemp 3
#define EEPROM_diffHumidity 10
#define EEPROM_diffTemperature 20
#define EEPROM_checkStatus 100

//display variables
#define introDelay    2000      //initial delay between intro and menu
#define brightenRate  30        //intro brighten speed (Higher value, slower)
#define valuePosition 245
#define separatorPosition 240
#define unitPosition 315
#define textFontSize 4          //text standard size

//configuration variables
#define maxPWMvalue 255         //for maple mini
#define maxHeaterPWM 150        //max power for heater, full power (255) is 50W
#define temperature_fraction 20 //times to measure in a 
#define mosfet_switch_time 50   //in millis, oversized
#define cornerNTC 0
#define heaterNTC 1
#define bothNTC 2
#define dhtSensor 2
#define numNTC 2
#define numTempSensors 3

//pin definition
//boardPWMPins: 3, 4, 5, 8, 9, 10, 11, 15, 16, 25, 26, 27

// Use hardware SPI lines+
//Pins from 4-6 are SPI
#define TFT_CS         7
#define TFT_DC         31
#define TFT_RST        2

#define DHTPIN 0
#define SCREENBACKLIGHT 3
#define THERMISTOR_HEATER 10
#define THERMISTOR_CORNER 11
#define ENCODER_A 12
#define ENCODER_B 13
#define POWER_EN 18
#define FAN1 9
#define FAN2 8
#define FAN3 15
#define HEATER 16
#define ICT 25
#define STERILIZE 28
#define WATERPUMP 27
#define HEATER_FB 17
#define POWER_EN_FB 19
#define FAN1_FB 20
#define FAN2_FB 21
#define FAN3_FB 22
#define ICT_FB 26
#define STERILIZE_FB 29
#define WATERPUMP_FB 30
#define pulse 14

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
#define COLOR_CHOSEN GREEN
#define COLOR_HEADING WHITE
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
const byte NTCpin[numNTC] = {THERMISTOR_HEATER, THERMISTOR_CORNER};
volatile int encstate[MAXENCODERS];
volatile int encflag[MAXENCODERS];
boolean A_set[MAXENCODERS];
boolean B_set[MAXENCODERS];
volatile int16_t encoderpos[MAXENCODERS];
volatile int  encodertimer = millis(); // acceleration measurement
int encoderpinA[MAXENCODERS] = {ENCODER_A}; // pin array of all encoder A inputs
int encoderpinB[MAXENCODERS] = {ENCODER_B}; // pin array of all encoder B inputs
unsigned int lastEncoderPos[MAXENCODERS];

int firmwareVersion = 0;

//Text position
int humidityX;
int humidityY;
int temperatureX;
int temperatureY;

//constants
const byte height = 40;
const byte separation = 10;
const byte height_bar = 200;
const byte height_letter = 70;
const byte rect_length = 80;
const byte heaterMaxTemp = 70;
const byte fanMaxSpeed = 100;
const byte LEDMaxIntensity = 100;
const byte time_back_draw = 255;
const byte time_back_wait = 255;
const byte minTemp = 15;
const byte maxTemp = 45;

int page, page0;
bool selected;
int data, instant_read;
byte text_size;
bool pos_text[8];
volatile int move;
long last_pulsed;
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
bool auto_lock;
byte counter;
byte language;
int text_height = tft.height() / 2;

double temperature[numTempSensors];
double previousTemperature[numTempSensors];
double desiredHeaterTemp;
int humidity;
double desiredIn3Temp = 35;
double heaterLimitTemp;
int fanSpeed;
int LEDIntensity;
long last_temp_update;
long temp_update_rate = 1000;
int backlight_intensity = 100;
bool enableSet;
float processPercentage = 0, temperatureAtStart;
int temperatureArray [numNTC][temperature_fraction];
byte temperaturePos, temperature_measured;
int barWidth, barHeight, barPosX, barPosY;
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
double Kp_in3 = 0.1, Ki_in3 = 0.3, Kd_in3 = 0.1;
double PIDOutput[2];
bool PIDControlStart;
PID heaterPID(&temperature[heaterNTC], &PIDOutput[heaterNTC], &desiredHeaterTemp, Kp_heater, Ki_heater, Kd_heater, P_ON_M, DIRECT);
PID in3PID(&temperature[cornerNTC], &PIDOutput[cornerNTC], &desiredIn3Temp, Kp_in3, Ki_in3, Kd_in3, P_ON_M, DIRECT);
// timer
#define PIDcontrol 0         //0 to disable, 1 to enable
#define ENCODER_RATE 1000    // in microseconds; 
#define NTCInterruptRate 20000    // in microseconds; 
#define in3PIDRate 1000000    // in microseconds; 
#define heaterPIDRate 200000   // times of in3PIDRate;
int in3PIDfactor = in3PIDRate / NTCInterruptRate;
int heaterPIDfactor = heaterPIDRate / NTCInterruptRate;
HardwareTimer timer(1);
HardwareTimer in3PIDTimer(2);

void setup() {
  Serial.begin(115200);
  dht.setup(DHTPIN);
  heaterPID.SetOutputLimits(0, maxHeaterPWM);
  tft.begin();
  tft.setRotation(1);
  loadLogo();
  pinDirection();
  initEEPROM();
  initPIDTimers();
  /*
    if (hardwareVerification()) {
    while (digitalRead(pulse));
    }
  */
  analogWrite(SCREENBACKLIGHT, backlight_intensity);
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
  pinMode(WATERPUMP, OUTPUT);
}
