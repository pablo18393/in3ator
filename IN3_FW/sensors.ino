void initSensors() {
  if (DHTSensor) {
    dht.setup(DHTPIN);
  }
  if (BME280Sensor) {
    bme.begin();
  }
  // timer setup for encoder
  initPulsioximeterVariables();
}

void sensorsISR() {
  measurenumNTC();
  if (millis() - lastSensorsUpdate > sensorsUpdateRate) {
    updateHumidity();
    lastSensorsUpdate = millis();
  }
  /*
    readPulsioximeter();
    if (!pulsioximeterCount) {
    pulsioximeterCount = pulsioximeterRate;
    calculatePulsioximeterValues();
    }
    pulsioximeterCount--;
  */
}

void initPulsioximeterVariables() {

}

void readPulsioximeter() {
  pulsioximeterInterSamples[pulsioximeterCount] = analogRead(PULSIOXIMETER);
  pulsioximeterMean *= (pulsioximeterMultiplierFilterFactor - pulsioximeterRestFilterFactor) / pulsioximeterMultiplierFilterFactor;
  pulsioximeterMean +=  pulsioximeterInterSamples[pulsioximeterCount] * pulsioximeterRestFilterFactor / pulsioximeterMultiplierFilterFactor;
}

void calculatePulsioximeterValues() {
  int meanPulsioximeterSamples;
  int pulsioximeterInterMean;
  for (int i = 0; i < pulsioximeterRate; i++) {
    pulsioximeterInterMean += pulsioximeterInterSamples[i] - pulsioximeterMean;
  }
  pulsioximeterInterMean /= pulsioximeterRate;
  pulsioximeterSample[pulsioximeterCounter[pulsioximeterSampled]][pulsioximeterSampled] = pulsioximeterInterMean;

  if (pulsioximeterCounter[pulsioximeterSampled] == maxPulsioximeterSamples) {
    pulsioximeterCounter[pulsioximeterSampled] = 0;
  }
  else {
    pulsioximeterCounter[pulsioximeterSampled]++;
  }
}


void measurenumNTC() {
  for (int i = 0; i < numNTC; i++) {
    temperatureArray[i][temperature_measured] = analogRead(NTCpin[i]);
  }
  temperature_measured++;
  if (temperature_measured == temperature_fraction) {
    updateTemp(numNTC);
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
    case babyNTC:
      startSensor = babyNTC;
      endSensor = babyNTC;
      break;
    case heaterNTC:
      startSensor = heaterNTC;
      endSensor = heaterNTC;
      break;
    case numNTC:
      startSensor = babyNTC;
      endSensor = inBoardRightNTC;
      break;
  }

  //Bloque de cálculo
  for (int i = startSensor; i <= endSensor; i++) {
    //Variables used in calculus
    float vm = 0.0;
    float rntc = 0.0;
    temperatureMean = 0;
    for (int j = 0; j < temperature_fraction; j++) {
      temperatureMean += temperatureArray[i][j];
    }
    temperatureMean /= temperature_fraction;
    vm = (vcc) * ( temperatureMean / maxADCvalue );          //Calcular tensión en la entrada
    rntc = rAux / ((vcc / vm) - 1);                   //Calcular la resistencia de la NTC
    temperature[i] = beta / (log(rntc / r0) + (beta / temp0)) - 273; //Calcular la temperatura en Celsius
    temperature[i] += diffTemperature[i];
  }
}

bool updateHumidity() {
  bool DHTOK = 0;
  bool BME280OK = 0;
  int DHTTemperature;
  int DHTHumidity;
  int BME280Temperature;
  int BME280Humidity;
  if (DHTSensor) {
    sensorsTimer.pause();
    DHTTemperature = dht.getTemperature();
    DHTHumidity = dht.getHumidity();
    sensorsTimer.resume();
    if (DHTHumidity && DHTTemperature && abs(DHTHumidity) <= 100) {
      temperature[digitalTempSensor] = DHTTemperature; //Add here measurement to temp array
      DHTOK = 1;
    }
  }
  if (BME280Sensor) {
    BME280Temperature = bme.readTemperature();
    BME280Humidity = bme.readHumidity();
    if (DHTHumidity && DHTTemperature) {
      temperature[digitalTempSensor] = BME280Temperature; //Add here measurement to temp array
      BME280OK = 1;
    }
  }
  if (DHTOK || BME280OK) {
    humidity = 0;
    humidity += DHTHumidity;
    humidity += BME280Humidity;
    humidity += diffHumidity;

  }
  if (DHTOK && BME280OK) {
    humidity -= diffHumidity;
    humidity /= 2;
    humidity += diffHumidity;
  }
  return (DHTOK || BME280OK);
}

void encoderISR() {
  if ( (digitalRead(ENC_A))  != A_set )
  {
    A_set = !A_set;
    if ( A_set && !B_set)
    {
      last_something = millis();
      EncMove = 1;
    }
  }
  if ( (digitalRead(ENC_B))  != B_set)
  {
    B_set = !B_set;
    if ( B_set && !A_set )
      EncMove = -1;
    last_something = millis();
  }
}

void asleep() {
  encPulsed = digitalRead(ENC_SWITCH);
  if (encPulsed != encPulsedBefore) {
    last_something = millis();
  }
  encPulsedBefore = encPulsed;
  if (auto_lock) {
    if (millis() - last_something > time_lock) {
      analogWrite(SCREENBACKLIGHT, 0);
    }
  }
  else {
    analogWrite(SCREENBACKLIGHT, TFT_LED);
  }
}
