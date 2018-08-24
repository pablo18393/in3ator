// ********PID function

void in3PIDInterrupt() {
  measureAllNTC();
  if (temperaturePIDcontrolStart) {
    if (interruptcounter == in3PIDfactor) {
      interruptcounter = 0;
      updateTemp(cornerNTC);
      in3PID.Compute();
      desiredHeaterTemp = PIDOutput[cornerNTC] * (heaterLimitTemp - desiredIn3Temp) / (maxPWMvalue) + desiredIn3Temp;  //map function: min value is 

    }
    if (interruptcounter % heaterPIDfactor == 0) {
      updateTemp(heaterNTC);
      heaterPID.Compute();
      analogWrite(HEATER, PIDOutput[heaterNTC]);
    }
    interruptcounter++;
  }
}

void initPIDTimers() {
  // PID setup for encoder
  in3PIDTimer.pause();
  in3PIDTimer.setPeriod(NTCInterruptRate); // in microseconds
  in3PIDTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  in3PIDTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  in3PIDTimer.attachCompare1Interrupt(in3PIDInterrupt);
  in3PIDTimer.refresh();
  in3PIDTimer.resume();

  in3PID.SetSampleTime(in3PIDRate / 1000);       //in milliseconds
  heaterPID.SetSampleTime(heaterPIDRate / 1000); //in milliseconds
}

void startPID() {
  in3PID.SetMode(AUTOMATIC);
  heaterPID.SetMode(AUTOMATIC);
  temperaturePIDcontrolStart = 1;
  interruptcounter = 0;
}

void stopPID() {
  in3PID.SetMode(MANUAL);
  heaterPID.SetMode(MANUAL);
  temperaturePIDcontrolStart = 0;
  analogWrite(HEATER, 0);
}

