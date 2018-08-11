// ********PID function
void PIDInterrupt() {

}

void initPID()
{
  // PID setup for encoder
  PID.pause();
  PID.setPeriod(PID_RATE); // in microseconds
  PID.setChannel2Mode(TIMER_OUTPUT_COMPARE);
  PID.setCompare(TIMER_CH2, 1);  // Interrupt 1 count after each update
  PID.attachCompare1Interrupt(PIDInterrupt);
  PID.refresh();
  PID.resume();
}
