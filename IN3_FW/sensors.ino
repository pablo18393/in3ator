
int myEncoderRead() {
  if ((lastEncoderPos[counter] != encoderpos[counter])) {
    encflag[counter] = LOW;
    lastEncoderPos[counter] = encoderpos[counter];
  }
  return (encoderpos[counter]);
}

void sensorsISR() {
  if (!encoderCount) {
    encoderCount = encoderRate;
    readEncoder();
  }
  encoderCount--;
  readPulsioximeter();
  if (!pulsioximeterCount) {
    pulsioximeterCount = pulsioximeterRate;
    calculatePulsioximeterValues();
  }
  pulsioximeterCount--;
}

void initPulsioximeterVariables() {

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
    //Variables used in calculus
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
    if (DHTHumidity && DHTTemperature) {
      //    temperature[numNTC + dhtSensor] = DHTTemperature; //Add here measurement to temp array
      DHTOK = 1;
    }
  }
  if (BME280Sensor) {
    BME280Temperature = bme.readTemperature();
    BME280Humidity = bme.readHumidity();
    if (DHTHumidity && DHTTemperature) {
      //    temperature[numNTC + dhtSensor] = DHTTemperature; //Add here measurement to temp array
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
    analogWrite(SCREENBACKLIGHT, backlight_intensity);
  }
}
