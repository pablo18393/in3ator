//Sensor check rate (in ms)
byte pulsioximeterRate = 5;
byte pulsioximeterCount = 0;
byte encoderRate = 1;
byte encoderCount = 0;

//sensor variables
int pulsioximeterMean;
int pulsioximeterFilterFactor = 3;


void initSensors()
{
  encodertimer = millis(); // acceleration measurement
  for (byte counter = 0; counter < MAXENCODERS; counter++)
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
    readpulsioximeter();
  }
  pulsioximeterCount--;
}

void readEncoder() {
  for (byte counter = 0; counter < MAXENCODERS; counter++)
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

void readPulsioximeter(){
  
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
