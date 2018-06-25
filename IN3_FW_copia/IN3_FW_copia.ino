const long tl_speed_travel = 200; //VELOCIDAD DE MOVIMIENTO DEL TIME LAPSE
const long tl_speed_pan = 4000; //VELOCIDAD DE MOVIMIENTO DEL TIME LAPSE
const int min_speed_travel = 900; //VELOCIDAD MINIMA DE MODO MANUAL
const byte max_speed_travel = 16; //VELOCIDAD MAXIMA MODO MANUAL
const long min_speed_pan = 40000; //VELOCIDAD MINIMA DE MODO MANUAL
const long max_speed_pan = 2000; //VELOCIDAD MAXIMA MODO MANUAL
byte acceleration = 3; //1%acceleration
byte acceleration_stop = 12;
int ramp_factor = 10;
int current_0 = 482;
int battery_max = 835;
int battery_min = 715;
const int t_min[3] = {250, 250, 250};
const float interval_margin = 1.4;
int battery_freq = 2000;
bool NuevoPrograma = 0; //solamente poner a 1 cuando se vaya a reprogramar un mando
byte numResonancia = 0; //introducir números de resonancia
byte velResonancia[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //introducir empezando desde la izquierda el número de la velocidad en la que hay resonancia
byte incResonancia[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //introducir empezando desde la izquierda el número de la incremento de velocidad (entre 5 y 20 suele ser notable)

/*
   EEPROM VARIABLES
   0: PAGE
   1: MANUAL MODE
   2: AUTO DATA
   3: AUTO DATA
   4: RAMP
   5: REPEAT
   6: SPEED
   7: TL DATA CLIP MIN
   8: TL DATA CLIP SEC
   9: TL DATA INTERVAL MIN
   10: TL DATA INTERVAL SEC
   11: TL DATA EXP MIN
   12: TL DATA EXP SEC
   13: AUTO LOCK
   14: CLIP FPS
   15: TL POWER
   16: DELAY_START
   17: LANGUAGE
   18:
   19:
   20:
   21: MAX SPEED
   22:TRAVEL COUNTER
   23:TRAVEL COUNTER
   24: PAN COUNTER
   25: PAN COUNTER
   170: EEPROM CHECK
   171-211: PAN SPEED
   212-252: TRAVEL SPEED
*/

/*page:
   2 manual
   3 auto mode
   4 time lapse
   5 settings
   6 stop mtion
   9-10 process page
   30 travelling/pan mode
*/
#include <Adafruit_GFX_AS.h>
#include <EEPROM.h>
#include <Adafruit_ILI9341_STM.h> // STM32 DMA Hardware-specific library
#include <SPI.h>

// Use hardware SPI lines+
#define TFT_CS         7
#define TFT_DC         31
#define TFT_RST        2

#define POWER_EN 18
#define POWER_EN_FB 19
#define BACKLIGHT1 3
#define BACKLIGHT2 23
#define FAN1 9
#define FAN2 8
#define FAN3 15
#define FAN1_FB 20
#define FAN2_FB 21
#define FAN3_FB 22
#define HEATER 16
#define HEATER_FB 17
#define ICT 25
#define ICT_FB 26
#define STERILIZE 28
#define STERILIZE_FB 29
#define WATERPUMP 27
#define WATERPUMP_FB 30
#define THERMISTOR1 11
#define THERMISTOR2 10
#define DHT22_1 24

#define pulse 14
#define LED1 3
#define LED2 3
#define LED3 3



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
#define COLOR_SELECTED WHITE
#define COLOR_CHOSEN GREEN
#define COLOR_HEADING WHITE
#define COLOR_ARROW 0x0000
#define COLOR_BATTERY BLACK
#define COLOR_BATTERY_LEFT BLACK

#define MAXENCODERS 1
volatile int encstate[MAXENCODERS];
volatile int encflag[MAXENCODERS];
boolean A_set[MAXENCODERS];
boolean B_set[MAXENCODERS];
volatile int16_t encoderpos[MAXENCODERS];
volatile int  encodertimer = millis(); // acceleration measurement
int encoderpinA[MAXENCODERS] = {12}; // pin array of all encoder A inputs
int encoderpinB[MAXENCODERS] = {13}; // pin array of all encoder B inputs
unsigned int lastEncoderPos[MAXENCODERS];

int data, instant_read;
byte page, page0;
byte text_size;
bool selected;
bool pos_text[8];
volatile int move;
long last_pulsed;
char* words[8];
byte bar_pos;
int oldPosition;
int newPosition;
byte rectangles;
bool manualMode;
const byte time_back_draw = 100;
const byte time_back_wait = 255;
const byte pos_clip[2] = {163, 220};
long min_interval;
const byte pos_dist = 130;
const byte pos_dolly = 175;
unsigned char letter;
bool mode;
long travel_counter, pan_counter;
byte tl_data[6], data_min, data_max;
int tl_distance;
byte distance_number[4];
bool tl_direction;
byte clip_fps = 28;
byte xpos;
byte length;
long stops, stops_done;
const byte height = 40;
const byte separation = 10;
const byte height_bar = 200;
const byte height_letter = 70;
const byte rect_length = 80;
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
bool state_asleep = 1;
bool auto_lock;
int manual_speed;
byte end;
byte counter;
bool repeat;
byte ramp;
byte am_speed;
bool am_move[2];
long check_EEPROM;
bool tl_power;
byte language;
byte delay_start;
bool keep_am;
byte color_led;
int voltage, current;
int battery_lines, battery_lines0;
byte battery_div = 4;
float battery_percentage = 0.96;
float battery_sure;
byte min_ramp, max_speed;
bool errorPan;
byte d;
bool stopped;
bool first_draw_battery = 1;
bool state_tl;
byte bar_limit;
byte shots_height;
byte text_height;
bool allow_shot;
bool battery_warning, battery_warning_0;
bool battery_blink;
long last_battery_blink;
bool waiting;
int led_intensity;
// timer
#define ENCODER_RATE 1000    // in microseconds; 
HardwareTimer timer(1);

void setup() {
  Serial3.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(pulse, INPUT_PULLUP);
  pinMode(ICT, OUTPUT);
  pinMode(22, INPUT);
  pinMode(11, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(PB1, OUTPUT);
  Serial.begin(115200);
  initEncoders();
  newPosition = myEncoderRead();
  auto_lock = EEPROM.read(13);
  oldPosition = newPosition;
  while (0) {
    /*
      digitalWrite(8,HIGH);
      digitalWrite(16,HIGH);
      delay(2000);
      digitalWrite(PB1,digitalRead(22));
      digitalWrite(8,LOW);
      digitalWrite(16,LOW);
      delay(2000);
      digitalWrite(PB1,digitalRead(22));
    */
    led_intensity += 10 * updateData();
    if (led_intensity > 255) {
      led_intensity = 255;
    }
    if (led_intensity < 0) {
      led_intensity = 0;
    }
    analogWrite(ICT, led_intensity);
  }

  tft.begin();
  tft.setRotation(1);
  initEEPROM();
  tft.fillScreen(WHITE);
  loadLogo();
  black();
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  //delay(2000);
  initEncoders();
  newPosition = myEncoderRead();
  auto_lock = EEPROM.read(13);
  oldPosition = newPosition;
  menu();
}
// the loop function runs over and over again forever
void loop() {

}

void initEEPROM() {

}

void recap() {
}

bool CheckComunication() {

}

byte read_char() {
  return (Serial3.read());
}

long read_string() {
  if (CheckComunication()) {
    data = 0;
    instant_read = 0;
    if (Serial3.peek() == '-') {
      Serial3.read();
    }
    while (Serial3.available() && instant_read != ',') {
      instant_read = read_char();
      if (instant_read != ',' && instant_read >= '0' && instant_read <= '9') {
        data = data * 10 + (instant_read - 48);
      }
    }
    return (data);
  }
  else {
    return (0);
  }
}

long EEPROMReadLong(int p_address)
{
  int lowByte = EEPROM.read(p_address);
  int highByte = EEPROM.read(p_address + 1);
  return ((lowByte << 0) & 0xFFFF) + ((highByte << 16) & 0xFFFF0000);
}
void EEPROMWriteLong(int p_address, long p_value)
{
  int lowByte = ((p_value >> 0) & 0xFFFFFFFF);
  int highByte = ((p_value >> 16) & 0xFFFFFFFF);

  EEPROM.write(p_address, lowByte);
  EEPROM.write(p_address + 1, highByte);
}
