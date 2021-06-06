
//PID VARIABLES
double Kp = 600, Ki = 150, Kd = 400;
double PIDOutput;
PID heatUPPID(&temperature[babyNTC], &PIDOutput, &desiredSkinTemp, Kp, Ki, Kd, P_ON_M, DIRECT);

void heatUPPIDISR() {
  if (heatUPPID.GetMode()) {
    heatUPPID.Compute();
    pwmWrite(HEATER, PIDOutput);
  }
}

void initHeaterPIDControl() {
  heatUPPID.SetOutputLimits(heaterMaxPWM * heaterPowerMin / 100, heaterMaxPWM * heaterPowerMax / 100);
  heatUPPID.SetTunings(Kp, Ki, Kd);
  configPIDTimer(PIDISRPeriod); //in micros
  heatUPPID.SetSampleTime(PIDISRPeriod / 1000); // in millis
  heatUPPID.SetControllerDirection(DIRECT);
}

void configPIDTimer(int freq) {
  //PID timer configuration:
  PIDTimer.pause();
  PIDTimer.setPeriod(freq); // in microseconds
  PIDTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  PIDTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  PIDTimer.attachCompare1Interrupt(heatUPPIDISR);
  PIDTimer.refresh();
  PIDTimer.resume();
}

void startHeaterPID() {
  initHeaterPIDControl();
  heatUPPID.SetMode(AUTOMATIC);
}

void stopHeaterPID() {
  heatUPPID.SetMode(MANUAL);
  pwmWrite(HEATER, 0);
}
