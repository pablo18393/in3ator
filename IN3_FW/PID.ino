
//PID VARIABLES
double Kp_heater = 0.1, Ki_heater = 0.3, Kd_heater = 0.1;
double Kp_room = 0.1, Ki_room = 0.3, Kd_room = 0.1;
double PIDOutput[2];
bool temperaturePIDcontrolStart;
PID heaterPID(&temperature[heaterNTC], &PIDOutput[heaterNTC], &desiredHeaterTemp, Kp_heater, Ki_heater, Kd_heater, P_ON_M, DIRECT);
PID roomPID(&temperature[roomNTC], &PIDOutput[roomNTC], &desiredSkinTemp, Kp_room, Ki_room, Kd_room, P_ON_M, DIRECT);

#define temperaturePIDcontrol 0         //0 to disable, 1 to enable
void roomPIDInterrupt() {
  measurenumNTC();
  if (temperaturePIDcontrolStart) {
    if (interruptcounter == roomPIDfactor) {
      interruptcounter = 0;
      updateTemp(roomNTC);
      roomPID.Compute();
      desiredHeaterTemp = PIDOutput[roomNTC] * (heaterTempLimit - desiredSkinTemp) / (maxPWMvalue) + desiredSkinTemp;  //map function: min value is
    }
    if (!(interruptcounter % heaterPIDfactor)) {
      updateTemp(heaterNTC);
      heaterPID.Compute();
      heaterPower = PIDOutput[heaterNTC];
      dac_write_channel(DAC, HEATER, heaterPower);
    }
    interruptcounter++;
  }
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
  dac_write_channel(DAC, HEATER, 0);
}
