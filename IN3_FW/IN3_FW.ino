/*
   EEPROM VARIABLES
   0: First turn on / eeprom format
   1: auto lock
   2: language
   3: diffTemperature
   4: diffHumidity
   5: heaterTemp
   6: fanSpeed
*/

/*page:
   0 menu
   1 processPage
*/
#include <Adafruit_GFX_AS.h>
#include <EEPROM.h>
#include <Adafruit_ILI9341_STM.h> // STM32 DMA Hardware-specific library
#include <SPI.h>
#include "DHT.h"

// Use hardware SPI lines+
#define TFT_CS         7
#define TFT_DC         31
#define TFT_RST        2

//configuration variables

#define temperature_fraction 20
#define mosfet_switch_time 50 //in millis, oversized
//pin definition
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
#define COLOR_MENU BLACK
#define COLOR_BAR  BLACK
#define COLOR_MENU_TEXT WHITE
#define COLOR_SELECTED WHITE
#define COLOR_CHOSEN GREEN
#define COLOR_HEADING WHITE
#define COLOR_ARROW 0x0000
#define COLOR_BATTERY BLACK
#define COLOR_BATTERY_LEFT BLACK
#define COLOR_FRAME_BAR WHITE
#define COLOR_LOADING_BAR RED
#define COLOR_COMPLETED_BAR GREEN
#define MAXENCODERS 1

DHT dht;

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

int page, page0;
bool selected;
int data, instant_read;
byte text_size;
bool pos_text[8];
volatile int move;
long last_pulsed;
char* words[8];
byte bar_pos;
int oldPosition;
int newPosition;
byte rectangles;
bool manualMode;
const byte time_back_draw = 255;
const byte time_back_wait = 255;
bool mode;
byte xpos;
byte length;
const byte height = 40;
const byte separation = 10;
const byte height_bar = 200;
const byte height_letter = 70;
const byte rect_length = 80;
const byte heaterMaxTemp = 70;
const byte fanMaxSpeed = 100;
const byte LEDMaxIntensity = 100;
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
int time_lock = 60000;
bool auto_lock;
byte counter;
byte language;
int text_height = tft.height() / 2;

float temperature;
int humidity;
float desiredTemp = 35;
int heaterTemp;
int fanSpeed;
int LEDIntensity;
long last_temp_update;
long temp_update_rate = 2000;
int backlight_intensity = 100;
bool enableSet;
float processPercentage = 0, temperatureAtStart;
int temperatureArray [temperature_fraction];
byte temperaturePos, temperature_measured;
int barWidth, barHeight, barPosX, barPosY;
byte barThickness;
float diffTemperature;
int diffHumidity;

// timer
#define ENCODER_RATE 1000    // in microseconds; 
HardwareTimer timer(1);

void setup() {
  Serial.begin(115200);
  dht.setup(DHTPIN);
  tft.begin();
  tft.setRotation(1);
  //loadLogo();
  pinDirection();
  initEEPROM();
  tft.fillScreen(WHITE);
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
