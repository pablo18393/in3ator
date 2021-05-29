#define testMode false
#define operativeMode true

#define NTC_BABY_MIN 0
#define NTC_BABY_MAX 60
#define DIG_TEMP_ROOM_MIN 0
#define DIG_TEMP_ROOM_MAX 60
#define DIG_HUM_ROOM_MIN 1
#define DIG_HUM_ROOM_MAX 100

#define HEATER_CONSUMPTION_MIN 2
#define FAN_CONSUMPTION_MIN 0.05
#define LEDS_CONSUMPTION_MIN 0.2
#define WATER_CONSUMPTION_MIN 0.05

#define HEATER_CONSUMPTION_MAX 9
#define FAN_CONSUMPTION_MAX 2
#define LEDS_CONSUMPTION_MAX 4
#define WATER_CONSUMPTION_MAX 1

#define STANDBY_CONSUMPTION_MIN 0.01
#define STANDBY_CONSUMPTION_MAX 1

#define SCREEN_CONSUMPTION_MIN 0.05
#define SCREEN_CONSUMPTION_MAX 1

#define NTC_BABY_MIN_ERROR 1
#define NTC_BABY_MAX_ERROR 1<<1
#define DIG_TEMP_ROOM_MIN_ERROR 1<<2
#define DIG_TEMP_ROOM_MAX_ERROR 1<<3
#define DIG_HUM_ROOM_MIN_ERROR 1<<4
#define DIG_HUM_ROOM_MAX_ERROR 1<<5
#define DIGITAL_SENSOR_NOTFOUND 1<<6

#define HEATER_CONSUMPTION_MIN_ERROR 1<<7
#define FAN_CONSUMPTION_MIN_ERROR    1<<8
#define LEDS_CONSUMPTION_MIN_ERROR   1<<9
#define WATER_CONSUMPTION_MIN_ERROR  1<<10

#define STANDBY_CONSUMPTION_MAX_ERROR 1<<11
#define DEFECTIVE_POWER_EN 1<<12
#define DEFECTIVE_SCREEN 1<<13
#define DEFECTIVE_BUZZER 1<<14
#define DEFECTIVE_CURRENT_SENSOR 1<<15

long HW_error = 0;

void initHardware() {
  logln("[HW] -> Initialiting hardware");
  initPins();
  checkCurrentSenseCircuit();
  initSensors();
  initTFT();
  initPowerEn();
  testActuators();
  if (!HW_error) {
    logln("[HW] -> HARDWARE OK");
    //GPRSSetPostVariables(addComent, "HW OK");
  }
  else {
    logln("[HW] -> HARDWARE TEST FAIL");
    logln("[HW] -> HARDWARE ERROR CODE:" + String(HW_error));
    //drawHardwareErrorMessage();
    /*
      while (digitalRead(ENC_SWITCH)) {
      updateData();
      }
    */
  }
}

void initPins() {
  logln("[HW] -> Initialiting pins");
  pinMode(SCREENBACKLIGHT, OUTPUT);
  pinMode(JAUNDICE, OUTPUT);
  pinMode(HEATER, OUTPUT);
  pinMode(FAN, OUTPUT);
  pinMode(BACKUP, OUTPUT);
  pinMode(ENC_SWITCH, INPUT_PULLUP);
  pinMode(POWER_EN, OUTPUT);
  pinMode(GPRS_PWRKEY, OUTPUT);
  pinMode(encoderpinA, INPUT_PULLUP);
  pinMode(encoderpinB, INPUT_PULLUP);
  pinMode(SYSTEM_SHUNT, INPUT);
  pinMode(PWR_ALERT, INPUT);
  pinMode(HUMIDIFIER, PWM);
  pinMode(BUZZER, PWM);
  pwmWrite(HUMIDIFIER, 0);
  pwmWrite(BUZZER, 0);
  digitalWrite(SCREENBACKLIGHT, HIGH);
  digitalWrite(POWER_EN, LOW);
  digitalWrite(JAUNDICE, LOW);
  digitalWrite(HEATER, LOW);
  digitalWrite(FAN, LOW);
  digitalWrite(GPRS_PWRKEY, HIGH);
}

void checkCurrentSenseCircuit() {
  long error = HW_error;
  standByCurrentTest();
  measureOffsetConsumption();
  if (error == HW_error) {
    logln("[HW] -> OK -> Current sensor is working as expected");
  }
}

void initSensors() {
  long error = HW_error;
  logln("[HW] -> Initialiting sensors");
  initRoomSensor();
  //sensors verification
  while (!measurenumNTC());
  if (temperature[babyNTC] < NTC_BABY_MIN) {
    logln("[HW] -> Fail -> NTC temperature is lower than expected");
    HW_error += NTC_BABY_MIN_ERROR;
  }
  if (temperature[babyNTC] > NTC_BABY_MAX) {
    logln("[HW] -> Fail -> NTC temperature is higher than expected");
    HW_error += NTC_BABY_MAX_ERROR;
  }
  if (updateRoomSensor()) {
    if (temperature[digitalTempSensor] < DIG_TEMP_ROOM_MIN) {
      logln("[HW] -> Fail -> Room temperature is lower than expected");
      HW_error += DIG_TEMP_ROOM_MIN_ERROR;
    }
    if (temperature[digitalTempSensor] > DIG_TEMP_ROOM_MAX) {
      logln("[HW] -> Fail -> Room temperature is higher than expected");
      HW_error += DIG_TEMP_ROOM_MAX_ERROR;
    }
    if (humidity < DIG_HUM_ROOM_MIN) {
      logln("[HW] -> Fail -> Room humidity is lower than expected");
      HW_error += DIG_HUM_ROOM_MIN_ERROR;
    }
    if (humidity > DIG_HUM_ROOM_MAX) {
      logln("[HW] -> Fail -> Room humidity is higher than expected");
      HW_error += DIG_HUM_ROOM_MAX_ERROR;
    }
  }
  else {
    HW_error += DIGITAL_SENSOR_NOTFOUND;
    logln("[HW] -> Fail -> No room sensor found");
  }
  if (error == HW_error) {
    logln("[HW] -> OK -> Sensors are working as expected");
  }
}

void initRoomSensor() {
  logln("[HW] -> Detecting room sensor...");
  Wire.begin();
  Wire.beginTransmission(roomSensorAddress);
  roomSensorPresent = Wire.endTransmission();
  if (roomSensorPresent == 0) {
    logln("[HW] -> Room sensor succesfully found, initializing...");
    mySHTC3.begin(Wire);
  }
  else {
    Wire.end();
  }
}

void standByCurrentTest() {
  logln("[HW] -> Measuring standby current...");
  if (sampleConsumption() < STANDBY_CONSUMPTION_MIN) {
    HW_error += DEFECTIVE_CURRENT_SENSOR;
    logln("[HW] -> Fail -> Defective current sensor");
  }
  if (sampleConsumption() > STANDBY_CONSUMPTION_MAX) {
    HW_error += STANDBY_CONSUMPTION_MAX_ERROR;
    logln("[HW] -> Fail -> Maximum stanby current exceeded");
  }
}

void initTFT() {
  long error = HW_error;
  SPI.setModule(SPI_SEL);
  tft.begin();
  tft.setRotation(3);
  loadlogo();
  if (sampleConsumption() < SCREEN_CONSUMPTION_MIN) {
    HW_error += DEFECTIVE_SCREEN;
    logln("[HW] -> Fail -> Screen current is not high enough");
  }
  if (sampleConsumption() > SCREEN_CONSUMPTION_MAX) {
    HW_error += DEFECTIVE_SCREEN;
    logln("[HW] -> Fail -> Screen current exceeded");
  }
  if (error == HW_error) {
    logln("[HW] -> OK -> Screen is working as expected");
  }
}

void initPowerEn() {
  long error = HW_error;
  logln("[HW] -> Checking power enable circuit...");
  digitalWrite(POWER_EN, HIGH);
  if (sampleConsumption() > STANDBY_CONSUMPTION_MAX) {
    HW_error += DEFECTIVE_POWER_EN;
    logln("[HW] -> Fail -> Defective power enable circuit");
  }
  if (error == HW_error) {
    logln("[HW] -> OK -> Power enable circuit is working as expected");
  }
}

void testActuators() {
}
