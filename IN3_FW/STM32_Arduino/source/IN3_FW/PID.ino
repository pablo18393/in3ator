//PID VARIABLES
#define tempCTL 0
#define humidityCTL 1
#define numPID 2

double Kp[numPID] = {30000, 200}, Ki[numPID] = {300, 2} , Kd[numPID] = {3000, 20};
double PIDOutput;
double temperaturePIDInput;
double humidityPIDOutput;
int WindowSize = 5000;
unsigned long windowStartTime;
PID temperaturePID(&temperaturePIDInput, &PIDOutput, &desiredSkinTemp, Kp[tempCTL], Ki[tempCTL], Kd[tempCTL], P_ON_E, DIRECT);
PID humidityPID(&humidity, &humidityPIDOutput, &desiredRoomHum, Kp[humidityCTL], Ki[humidityCTL], Kd[humidityCTL], P_ON_E, DIRECT);

void PIDISR() {
  if (temperaturePID.GetMode() == AUTOMATIC) {
    if (controlMode) {
      temperaturePIDInput = temperature[airNTC];
    }
    else {
      temperaturePIDInput = temperature[babyNTC];
    }
    temperaturePID.Compute();
    pwmWrite(HEATER, PIDOutput);
  }
  if (humidityPID.GetMode() == AUTOMATIC) {
    humidityPID.Compute();
    if (millis() - windowStartTime > WindowSize)
    { //time to shift the Relay Window
      windowStartTime += WindowSize;
    }
    if (humidityPIDOutput < millis() - windowStartTime) {
      digitalWrite(HUMIDIFIER, LOW);
    }
    else {
      digitalWrite(HUMIDIFIER, HIGH);
    }
  }
}

void configPIDTimer(int freq) {
  //PID timer configuration:
  PIDTimer.pause();
  PIDTimer.setPeriod(freq); // in microseconds
  PIDTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  PIDTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  PIDTimer.attachCompare1Interrupt(PIDISR);
  PIDTimer.refresh();
  PIDTimer.resume();
}

void startTemperaturePID() {
  temperaturePID.SetOutputLimits(heaterMaxPWM * heaterPowerMin / 100, heaterMaxPWM * heaterPowerMax / 100);
  temperaturePID.SetTunings(Kp[tempCTL], Ki[tempCTL], Kd[tempCTL]);
  temperaturePID.SetControllerDirection(DIRECT);
  temperaturePID.SetMode(AUTOMATIC);
  configPIDTimer(PIDISRPeriod); //in micros
}

void stopTemperaturePID() {
  temperaturePID.SetMode(MANUAL);
  pwmWrite(HEATER, LOW);
  PIDTimer.pause();
}

void startHumidityPID() {
  windowStartTime = millis();
  humidityPID.SetOutputLimits(0, WindowSize);
  humidityPID.SetMode(AUTOMATIC);
  configPIDTimer(PIDISRPeriod); //in micros
}

void stopHumidityPID() {
  humidityPID.SetMode(MANUAL);
  digitalWrite(HUMIDIFIER, LOW);
  PIDTimer.pause();
}
