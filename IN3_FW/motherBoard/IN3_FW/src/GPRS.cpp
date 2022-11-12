/*
  MIT License

  Copyright (c) 2022 Medical Open World, Pablo Sánchez Bergasa

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/

#include <Arduino.h>
#include "GPRS.h"
#include "main.h"
#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>
#include "ThingsBoard.h"

// Initialize GSM modem
TinyGsm modem(modemSerial);

// Initialize GSM client
TinyGsmClient client(modem);

// Initialize ThingsBoard instance
ThingsBoard tb(client);

extern TwoWire *wire;
extern MAM_in3ator_Humidifier in3_hum;
extern Adafruit_ILI9341 tft;
extern SHTC3 mySHTC3; // Declare an instance of the SHTC3 class
extern RotaryEncoder encoder;
extern Beastdevices_INA3221 digitalCurrentSensor;

extern int serialNumber;

extern bool WIFI_EN;
extern bool defaultWIFI_EN;
extern long lastDebugUpdate;
extern long loopCounts;
extern int page;
extern byte language;
extern int temperature_filter; // amount of temperature samples to filter
extern long lastNTCmeasurement, lastCurrentMeasurement, lastCurrentUpdate;

extern int NTC_PIN[numNTC];
extern double temperature[numSensors];
extern double errorTemperature[numSensors], temperatureCalibrationPoint;
extern double ReferenceTemperatureRange, ReferenceTemperatureLow;
extern double provisionalReferenceTemperatureLow;
extern double fineTuneSkinTemperature;
extern double RawTemperatureLow[numSensors], RawTemperatureRange[numSensors];
extern double provisionalRawTemperatureLow[numSensors];
extern double temperatureMaxReset;
extern double temperatureMinReset;
extern double temperatureMax[numSensors], temperatureMin[numSensors];
extern int temperatureArray[numNTC][analog_temperature_filter]; // variable to handle each NTC with the array of last samples (only for NTC)
extern int temperature_array_pos;                               // temperature sensor number turn to measure
extern float diffTemperature;                                   // difference between measured temperature and user input real temperature
extern bool faultNTC[numNTC];                                   // variable to control a failure in NTC
extern double humidity;                                         // room humidity variable
extern bool humidifierState, humidifierStateChange;
extern int previousHumidity; // previous sampled humidity
extern float diffHumidity;   // difference between measured humidity and user input real humidity

extern byte autoCalibrationProcess;

// Sensor check rate (in ms). Both sensors are checked in same interrupt and they have different check rates
extern byte encoderRate;
extern byte encoderCount;
extern bool encPulseDetected;
extern volatile long lastEncPulse;
extern volatile bool statusEncSwitch;

// WIFI
extern bool WIFI_connection_status;

extern bool roomSensorPresent;
extern bool digitalCurrentSensorPresent;

extern float instantTemperature[secondOrder_filter];
extern float previousTemperature[secondOrder_filter];

// room variables
extern bool controlMode;
extern bool defaultcontrolMode;
extern bool controlAlgorithm;
extern bool defaultcontrolAlgorithm;
extern double desiredControlTemperature; // preset baby skin temperature
extern double desiredControlHumidity;    // preset enviromental humidity
extern bool jaundiceEnable;              // PWM that controls jaundice LED intensity
extern double desiredHeaterTemp;         // desired temperature in heater

extern boolean A_set;
extern boolean B_set;
extern int encoderpinA;                 // pin  encoder A
extern int encoderpinB;                 // pin  encoder B
extern bool encPulsed, encPulsedBefore; // encoder switch status
extern bool updateUIData;
extern volatile int EncMove;            // moved encoder
extern volatile int lastEncMove;        // moved last encoder
extern volatile int EncMoveOrientation; // set to -1 to increase values clockwise
extern int last_encoder_move;           // moved encoder
extern long encoder_debounce_time;      // in milliseconds, debounce time in encoder to filter signal bounces
extern long last_encPulsed;             // last time encoder was pulsed

// Text Graphic position variables
extern int humidityX;
extern int humidityY;
extern int temperatureX;
extern int temperatureY;
extern int separatorTopYPos;
extern int separatorBotYPos;
extern bool controlTemperature;
extern bool controlHumidity;
extern int ypos;
extern bool print_text;
extern int initialSensorPosition;
extern bool pos_text[8];

extern bool enableSet;
extern float temperaturePercentage, temperatureAtStart;
extern float humidityPercentage, humidityAtStart;
extern int barWidth, barHeight, tempBarPosX, tempBarPosY, humBarPosX, humBarPosY;
extern int screenTextColor, screenTextBackgroundColor;

// User Interface display variables
extern bool autoLock;             // setting that enables backlight switch OFF after a given time of no user actions
extern bool defaultAutoLock;      // setting that enables backlight switch OFF after a given time of no user actions
extern int time_lock;             // time to lock screen if no user actions
extern long lastbacklightHandler; // last time there was a encoder movement or pulse
extern long sensorsUpdatePeriod;

extern bool selected;
extern char cstring[128];
extern char *textToWrite;
extern char *words[12];
extern char *helpMessage;
extern byte bar_pos;
extern byte menu_rows;
extern byte length;
extern long lastGraphicSensorsUpdate;
extern long lastSensorsUpdate;
extern bool enableSetProcess;
extern long blinking;
extern bool state_blink;
extern bool blinkSetMessageState;
extern long lastBlinkSetMessage;

extern bool powerAlert;
extern long lastSuccesfullSensorUpdate[numSensors];

extern double HeaterPIDOutput;
extern double skinControlPIDInput;
extern double airControlPIDInput;
extern double humidityControlPIDOutput;
extern int humidifierTimeCycle;
extern unsigned long windowStartTime;

extern double Kp[numPID], Ki[numPID], Kd[numPID];
extern PID airControlPID;
extern PID skinControlPID;
extern PID humidityControlPID;

long lastGPRSRoutine;

String user = "admin@admin.com";
String password = "admin";
String server = "pub.scar.io";

int len = false;

struct GPRSstruct
{
  bool enable;
  bool firstPost;
  bool postSN;
  bool postBabyTemp;
  bool postAirTemp;
  bool postBoardTemp1;
  bool postBoardTemp2;
  bool postBoardTemp3;
  bool postHumidity;
  bool postLongitud;
  bool postLatitud;
  bool postJaundicePower;
  bool postRSSI;
  bool postCurrentConsumption;
  bool postComment;
  int sendPeriod;
  long lastSent;
  char buffer[RX_BUFFER_LENGTH];
  int charToRead;
  int bufferPos;
  int bufferWritePos;
  String latitud;
  String longitud;
  String localDayTime;
  String localHourTime;
  String comment;
  String RSSI;
  bool readLatitud;
  bool readLongitud;
  bool readLocalDayTime;
  bool readLocalHourTime;
  String token;
  String reply;
  long lastEvent;
  bool processSuccess;
  bool readToken;
  bool powerUp;
  bool connect;
  bool connectionStatus;
  bool timeOut;
  byte process;
  long processTime;
  long packetSentenceTime;
  bool post;
  byte postProcess;
  bool validToken;
};

int GPRSsequence = false;

struct GPRSstruct GPRS;

void clearGPRSBuffer()
{
  memset(GPRS.buffer, 0, sizeof(GPRS.buffer));
  GPRS.charToRead = false;
  GPRS.bufferPos = false;
  GPRS.bufferWritePos = false;
}

String checkSerial(const char *success, const char *error)
{
  if (strstr(GPRS.buffer, success))
  {
    logln(GPRS.reply);
    GPRS.process++;
    clearGPRSBuffer();
    return (GPRS.reply);
  }
  if (strstr(GPRS.buffer, success))
  {
    logln("[GPRS] -> GPRS error: " + String(error));
    GPRS.processSuccess = false;
    GPRS.process++;
    clearGPRSBuffer();
  }
  return "";
}

void initGPRS()
{
  Serial2.begin(115200);
  GPRS.enable = true;
  GPRS.powerUp = true;
  setGPRSPostPeriod(standByGPRSPostPeriod);
  digitalWrite(GPRS_PWRKEY, HIGH);
}

void GPRSStatusHandler()
{
  if (GPRS.process)
  {
    if (GPRS.powerUp || GPRS.connect || GPRS.post)
    {
      if (millis() - GPRS.processTime > GPRS_TIMEOUT)
      {
        logln("[GPRS] -> timeOut: " + String(GPRS.powerUp) + String(GPRS.connect) + String(GPRS.post) + String(GPRS.process));
        GPRS.timeOut = false;
        GPRS.process = false;
        GPRS.post = false;
        GPRS.connect = false;
        GPRS.powerUp = true;
        GPRS.validToken = false;
        logln("[GPRS] -> powering module down...");
        Serial2.print("AT+CPOWD=1\n");
        GPRS.packetSentenceTime = millis();
        GPRS.processTime = millis();
      }
    }
  }
  if (!GPRS.powerUp && !GPRS.connect && !GPRS.post)
  {
    if (!GPRS.connectionStatus)
    {
      GPRS.powerUp = true;
    }
  }
}

void GPRSPowerUp()
{
  switch (GPRS.process)
  {
  case 0:
    if (millis() - GPRS.packetSentenceTime > 2000)
    {
      GPRS.processTime = millis();
      digitalWrite(GPRS_PWRKEY, LOW);
      GPRS.processSuccess = true;
      GPRS.process++;
      GPRS.packetSentenceTime = millis();
      logln("[GPRS] -> powering up GPRS");
    }
    break;
  case 1:
    if (millis() - GPRS.packetSentenceTime > 2000)
    {
      digitalWrite(GPRS_PWRKEY, HIGH);
      GPRS.packetSentenceTime = millis();
      GPRS.process++;
      logln("[GPRS] -> GPRS powered");
    }
    break;
  case 2:
    if (millis() - GPRS.packetSentenceTime > 7000)
    {
      clearGPRSBuffer();
      logln("[GPRS] -> Sending AT command");
      Serial2.print(SIMCOM800_AT);
      GPRS.packetSentenceTime = millis();
    }
    checkSerial(AT_OK, AT_ERROR);
    break;
  case 3:
    if (GPRS.processSuccess)
    {
      GPRS.powerUp = false;
      GPRS.connect = true;
      logln("[GPRS] -> Power up success");
    }
    GPRS.process = false;
    break;
  }
}

void GPRSStablishConnection()
{
  switch (GPRS.process)
  {
  case 0:
    logln("[GPRS] -> Stablishing connection");
    GPRS.processTime = millis();
    GPRS.processSuccess = true;
    GPRS.packetSentenceTime = millis();
    GPRS.process++;
    break;
  case 1:
    if (millis() - GPRS.processTime > 5000)
    {
      logln("[GPRS] -> Connecting");

      modem.gprsConnect(APN, GPRS_USER, GPRS_PASS);
      GPRS.process++;
      logln("[GPRS] -> Finished");
    }
    break;
  case 2:
    if (GPRS.processSuccess)
    {
      GPRS.connectionStatus = true;
    }
    else
    {
      GPRS.powerUp = true;
    }
    GPRS.connect = false;
    GPRS.process = false;
    GPRS.post = true;
    break;
  }
}

void readGPRSData()
{
  while (Serial2.available())
  {
    GPRS.buffer[GPRS.bufferWritePos] = Serial2.read();
    Serial.print(GPRS.buffer[GPRS.bufferWritePos]);
    GPRS.bufferWritePos++;
    if (GPRS.bufferWritePos >= RX_BUFFER_LENGTH)
    {
      GPRS.bufferWritePos = 0;
      logln("[GPRS] -> Buffer overflow");
    }
    GPRS.charToRead++;
  }
}

void GPRSSetPostVariables(byte postContent, String postComment)
{
}

void setGPRSPostPeriod(long seconds)
{
  GPRS.sendPeriod = seconds;
}

bool GPRSLoadVariables()
{
  /*
  databasePost[7] = "{";
  databasePost[7] += "\"sn\":\"" + String(serialNumber) + "\"";
  if (GPRS.postBabyTemp)
  {
    databasePost[7] += ",\"baby_temp\":\"" + String(temperature[skinSensor], 1) + "\"";
  }
  if (GPRS.postAirTemp)
  {
    databasePost[7] += ",\"heater_temp\":\"" + String(temperature[airSensor], 1) + "\"";
  }
  if (GPRS.postBoardTemp1)
  {
    // databasePost[7] += ",\"board_temp1\":\"" + String(temperature[inBoardLeftNTC], 2) + "\"";
  }
  if (GPRS.postBoardTemp2)
  {
    // databasePost[7] += ",\"board_temp2\":\"" + String(temperature[inBoardRightNTC], 2) + "\"";
  }
  if (GPRS.postBoardTemp3)
  {
    // databasePost[7] += ",\"board_temp3\":\"" + String(temperature[airSensor], 1) + "\"";
  }
  if (GPRS.postHumidity)
  {
    databasePost[7] += ",\"humidity\":\"" + String(humidity, 0) + "\"";
  }
  if (GPRS.postLongitud)
  {
    databasePost[7] += ",\"long\":\"" + GPRS.longitud + "\"";
  }
  if (GPRS.postLatitud)
  {
    databasePost[7] += ",\"lat\":\"" + GPRS.latitud + "\"";
  }
  if (GPRS.RSSI)
  {
    databasePost[7] += ",\"rssi\":\"" + GPRS.RSSI + "\"";
  }
  if (GPRS.postJaundicePower)
  {
    databasePost[7] += ",\"jaundice_power\":\"" + String(jaundiceEnable) + "\"";
  }
  if (GPRS.postCurrentConsumption)
  {
    databasePost[7] += ",\"power\":\"" + String(measureMeanConsumption(SYSTEM_SHUNT_CHANNEL), 1) + "\"";
    // databasePost[7] += ",\"power_usb\":\"" + String(currentConsumption[HUMIDIFIER_SHUNT], 2) + "\"";
  }
  if (GPRS.postComment)
  {
    if (GPRS.comment.length() < 20)
    {
      GPRS.comment += "Baby " + String(temperatureMax[skinSensor], 2) + "/" + String(temperatureMin[skinSensor], 2);
      GPRS.comment += ";Air " + String(temperatureMax[airSensor], 2) + "/" + String(temperatureMin[airSensor], 2);
      GPRS.comment += "CM:" + String(controlMode);
      temperatureMax[skinSensor] = temperatureMaxReset;
      temperatureMin[skinSensor] = temperatureMinReset;
      temperatureMax[airSensor] = temperatureMaxReset;
      temperatureMin[airSensor] = temperatureMinReset;
    }
    databasePost[7] += ",\"comments\":\"" + GPRS.comment + "\"";
  }
  databasePost[7] += "}\n";
  logln("[GPRS] -> Posting: " + databasePost[7]);
  if (databasePost[7].indexOf("nan") >= 0)
  {
    logln("[GPRS] -> Detected zero");
    return false;
  }
  if (databasePost[7].length() > 1200)
  {
    logln("[GPRS] -> posting packet size oversized");
    return false;
  }
  return true;
  */
}

void GPRSPost()
{

  if (!tb.connected())
  {
    // Connect to the ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN))
    {
      Serial.println("Failed to connect");
      return;
    }
    delay(1000);
  }
  if (tb.connected() && millis() - GPRS.lastSent > millisToSecs(GPRS.sendPeriod))
  {
    logln("[GPRS] -> Posting GPRS data...");


    GPRS.comment = "";
    GPRS.process = false;
    GPRS.lastSent = millis();
    logln("[GPRS] -> GPRS POST SUCCESS");
  }
}

void GPRS_Handler()
{
  if (GPRS.enable)
  {
    if (GPRS.powerUp)
    {
      GPRSPowerUp();
    }
    if (GPRS.connect)
    {
      GPRSStablishConnection();
    }
    if (GPRS.post)
    {
      GPRSPost();
    }
    readGPRSData();
  }
}