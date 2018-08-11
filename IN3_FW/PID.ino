// ********PID function
void heaterPIDInterrupt() {
  heaterPID.Compute();
}

void in3PIDInterrupt() {
  in3PID.Compute();
}

void initPID()
{
  // PID setup for encoder
  heaterPIDTimer.pause();
  heaterPIDTimer.setPeriod(heaterPIDRate); // in microseconds
  heaterPIDTimer.setChannel2Mode(TIMER_OUTPUT_COMPARE);
  heaterPIDTimer.setCompare(TIMER_CH2, 1);  // Interrupt 1 count after each update
  heaterPIDTimer.attachCompare1Interrupt(heaterPIDInterrupt);
  heaterPIDTimer.refresh();
  heaterPIDTimer.resume();

  // PID setup for encoder
  in3PIDTimer.pause();
  in3PIDTimer.setPeriod(in3PIDRate); // in microseconds
  in3PIDTimer.setChannel2Mode(TIMER_OUTPUT_COMPARE);
  in3PIDTimer.setCompare(TIMER_CH2, 1);  // Interrupt 1 count after each update
  in3PIDTimer.attachCompare1Interrupt(in3PIDInterrupt);
  in3PIDTimer.refresh();
  in3PIDTimer.resume();


}
