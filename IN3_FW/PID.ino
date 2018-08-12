// ********PID function

void in3PIDInterrupt() {
  updateTemp(heaterNTC);
  heaterPID.Compute();
  if (interruptcounter == heaterPIDRate) {
    interruptcounter = 0;
    updateTemp(cornerNTC);
    in3PID.Compute();
  }
  interruptcounter++;
}

void initPIDTimers() {
  // PID setup for encoder
  in3PIDTimer.pause();
  in3PIDTimer.setPeriod(in3PIDRate); // in microseconds
  in3PIDTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  in3PIDTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  in3PIDTimer.attachCompare1Interrupt(in3PIDInterrupt);


  heaterPID.SetSampleTime(heaterPIDRate * in3PIDRate / 1000);
  in3PID.SetSampleTime(in3PIDRate / 1000);
}

void startPID() {
  heaterPID.SetMode(AUTOMATIC);
  in3PID.SetMode(AUTOMATIC);
  in3PIDTimer.refresh();
  in3PIDTimer.resume();
}

void stopPID() {
  heaterPID.SetMode(MANUAL);
  in3PID.SetMode(MANUAL);
  in3PIDTimer.pause();
}

