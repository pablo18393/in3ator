//temperature variables
#define CheckSensorRaiseTemp 3   //in celsius degrees, the amount of degrees to differentiate heater or room NTC
#define roomNTC 0
#define heaterNTC 1
#define inBoardLeftNTC 2
#define inBoardRightNTC 3
#define dhtSensor 1
#define numNTC 4
#define numTempSensors 5
#define temperature_fraction 20 //numbers of measures of the temperatures sensors

int temperatureArray [numNTC][temperature_fraction];
bool faultNTC[numNTC];
byte heaterPower;
byte numSensors;
double temperature[numTempSensors];
double previousTemperature[numTempSensors];
int humidity;
int previousHumidity;

//Sensor check rate (in ms)
byte pulsioximeterRate = 5;
byte pulsioximeterCount = 0;
byte encoderRate = 1;
byte encoderCount = 0;

//sensor variables
int pulsioximeterMean;
int pulsioximeterFilterFactor = 3;

//constants
const byte heaterMaxTemp = 100;
const byte minTemp = 15;
const byte maxTemp = 45;
const byte maxHum = 100;
const byte minHum = 20;

//Encoder variables
#define NUMENCODERS 1
byte NTCpin[numNTC] = {THERMISTOR_HEATER, THERMISTOR_ROOM, THERMISTOR_INBOARD_LEFT, THERMISTOR_INBOARD_RIGHT};
volatile int encstate[NUMENCODERS];
volatile int encflag[NUMENCODERS];
boolean A_set[NUMENCODERS];
boolean B_set[NUMENCODERS];
volatile int16_t encoderpos[NUMENCODERS];
volatile int  encodertimer = millis(); // acceleration measurement
int encoderpinA[NUMENCODERS] = {ENCODER_A}; // pin array of all encoder A inputs
int encoderpinB[NUMENCODERS] = {ENCODER_B}; // pin array of all encoder B inputs
unsigned int lastEncoderPos[NUMENCODERS];
int EncOldPosition;
int EncNewPosition;

void initSensors() {
  encodertimer = millis(); // acceleration measurement
  for (byte counter = 0; counter < NUMENCODERS; counter++)
  {
    encstate[counter] = HIGH;
    encflag[counter] = HIGH;
    A_set[counter] = false;
    B_set[counter] = false;
    encoderpos[counter] = 0;
    pinMode(encoderpinA[counter], INPUT_PULLUP);
    pinMode(encoderpinB[counter], INPUT_PULLUP);
    lastEncoderPos[counter] = 1;
  }
  // timer setup for encoder
  sensorsTimer.pause();
  sensorsTimer.setPeriod(sensorsRate); // in microseconds
  sensorsTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  sensorsTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  sensorsTimer.attachCompare1Interrupt(readSensors);
  sensorsTimer.refresh();
  sensorsTimer.resume();
}

int myEncoderRead() {
  if ((lastEncoderPos[counter] != encoderpos[counter])) {
    encflag[counter] = LOW;
    lastEncoderPos[counter] = encoderpos[counter];
  }
  return (encoderpos[counter]);
}

void readSensors() {
  if (!encoderCount) {
    encoderCount = encoderRate;
    readEncoder();
  }
  encoderCount--;
  if (!pulsioximeterCount) {
    pulsioximeterCount = pulsioximeterRate;
    readPulsioximeter();
  }
  pulsioximeterCount--;
}

void readEncoder() {
  for (byte counter = 0; counter < NUMENCODERS; counter++)
  {
    if ( (gpio_read_bit(PIN_MAP[encoderpinA[counter]].gpio_device, PIN_MAP[encoderpinA[counter]].gpio_bit) ? HIGH : LOW) != A_set[counter] )
    {
      A_set[counter] = !A_set[counter];
      if ( A_set[counter] && !B_set[counter] )
      {
        if (millis() - encodertimer < -1)
          encoderpos[counter] += 1;
        else
          encoderpos[counter] += 5;
      }
      encodertimer = millis();
      last_something = millis();
    }
    if ( (gpio_read_bit(PIN_MAP[encoderpinB[counter]].gpio_device, PIN_MAP[encoderpinB[counter]].gpio_bit) ? HIGH : LOW) != B_set[counter] )
    {
      B_set[counter] = !B_set[counter];
      if ( B_set[counter] && !A_set[counter] )
        if (millis() - encodertimer < -1)
          encoderpos[counter] -= 1;
        else
          encoderpos[counter] -= 5;
      encodertimer = millis();
      last_something = millis();
    }
  }
  asleep();
}

void readPulsioximeter() {

}

void measurenumNTC() {
  for (int ntc = 0; ntc < numNTC; ntc++) {
    temperatureArray[ntc][temperature_measured] = analogRead(NTCpin[ntc]);
  }
  temperature_measured++;
  if (temperature_measured == temperature_fraction) {
    temperature_measured = 0;
  }
}

void updateTemp(byte sensor) {
  //Valores fijos del circuito
  float rAux = 10000.0;
  float vcc = 3.3;
  float beta = 3950.0;
  float temp0 = 298.0;
  float r0 = 10000.0;
  float temperatureMean;
  byte startSensor;
  byte endSensor;

  switch (sensor) {
    case roomNTC:
      startSensor = roomNTC;
      endSensor = roomNTC;
      break;
    case heaterNTC:
      startSensor = heaterNTC;
      endSensor = heaterNTC;
      break;
    case numNTC:
      startSensor = roomNTC;
      endSensor = heaterNTC;
      break;
  }

  //Bloque de cálculo
  for (int ntc = startSensor; ntc <= endSensor; ntc++) {
    //Variables usadas en el cálculo
    float vm = 0.0;
    float rntc = 0.0;
    temperatureMean = 0;
    for (int i = 0; i < temperature_fraction; i++) {
      temperatureMean += temperatureArray[ntc][i];
    }
    temperatureMean /= temperature_fraction;
    vm = (vcc) * ( temperatureMean / maxADCvalue );          //Calcular tensión en la entrada
    rntc = rAux / ((vcc / vm) - 1);                   //Calcular la resistencia de la NTC
    temperature[ntc] = beta / (log(rntc / r0) + (beta / temp0)) - 273; //Calcular la temperatura en Celsius
    temperature[ntc] += diffTemperature[ntc];
  }
}

bool readHumSensor() {
  sensorsTimer.pause();
  int newTemp = dht.getTemperature();
  int newHumidity = dht.getHumidity();
  sensorsTimer.refresh();
  sensorsTimer.resume();
  if (newHumidity && newTemp) {
    //    temperature[numNTC + dhtSensor] = newTemp;
    humidity = newHumidity;
    humidity += diffHumidity;
    return (1);
  }
  return (0);
}

void asleep() {
  pulsed = digitalRead(pulse);
  if (pulsed != pulsed_before) {
    last_something = millis();
  }
  pulsed_before = pulsed;
  if (auto_lock) {
    if (millis() - last_something > time_lock) {
      analogWrite(SCREENBACKLIGHT, 0);
    }
  }
  else {
    analogWrite(SCREENBACKLIGHT, backlight_intensity);
  }
}
