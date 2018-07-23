// ********encoder function
int myEncoderRead() {
  if ((lastEncoderPos[counter] != encoderpos[counter])) {
    encflag[counter] = LOW;
    lastEncoderPos[counter] = encoderpos[counter];
  }
  return (encoderpos[counter]);
}
void asleep() {
  pulsed = digitalRead(pulse);
  if (pulsed != pulsed_before) {
    last_something = millis();
  }
  pulsed_before = pulsed;

  if (millis() - last_something > time_lock && !auto_lock) {
    //digitalWrite(LED1, LOW);
    //digitalWrite(LED2, LOW);
    //digitalWrite(LED3, LOW);
    state_asleep = 0;
  }
  else {
    //digitalWrite(LED1, HIGH);
    //digitalWrite(LED2, HIGH);
    //digitalWrite(LED3, HIGH);
    delayMicroseconds(1000);
    state_asleep = 1;
  }
}
void readEncoders() {
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

void initEncoders()
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
  timer.pause();
  timer.setPeriod(ENCODER_RATE); // in microseconds
  timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  timer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  timer.attachCompare1Interrupt(readEncoders);
  timer.refresh();
  timer.resume();
}
