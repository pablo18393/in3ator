// ********PID function

void roomPIDInterrupt() {
  measureAllNTC();
  if (temperaturePIDcontrolStart) {
    if (interruptcounter == roomPIDfactor) {
      interruptcounter = 0;
      updateTemp(roomNTC);
      roomPID.Compute();
      desiredHeaterTemp = PIDOutput[roomNTC] * (heaterLimitTemp - desiredRoomTemp) / (maxPWMvalue) + desiredRoomTemp;  //map function: min value is 
    }
    if (!(interruptcounter % heaterPIDfactor)) {
      updateTemp(heaterNTC);
      heaterPID.Compute();
      analogWrite(HEATER, PIDOutput[heaterNTC]);
    }
    interruptcounter++;
  }
}

void initPIDTimers() {
  // PID setup for encoder
  roomPIDTimer.pause();
  roomPIDTimer.setPeriod(NTCInterruptRate); // in microseconds
  roomPIDTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  roomPIDTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  roomPIDTimer.attachCompare1Interrupt(roomPIDInterrupt);
  roomPIDTimer.refresh();
  roomPIDTimer.resume();

  roomPID.SetSampleTime(roomPIDRate / 1000);       //in milliseconds
  heaterPID.SetSampleTime(heaterPIDRate / 1000); //in milliseconds
  heaterPID.SetOutputLimits(0, maxHeaterPWM);
}

void startPID() {
  roomPID.SetMode(AUTOMATIC);
  heaterPID.SetMode(AUTOMATIC);
  temperaturePIDcontrolStart = 1;
  interruptcounter = 0;
}

void stopPID() {
  roomPID.SetMode(MANUAL);
  heaterPID.SetMode(MANUAL);
  temperaturePIDcontrolStart = 0;
  analogWrite(HEATER, 0);
}

