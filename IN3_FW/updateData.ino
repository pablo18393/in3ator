void updateData() {
  checkSleep();
  newPosition = myEncoderRead();
  move = oldPosition - newPosition;
  if (abs(move) > 1) {
    if (move > 0) {
      move = 1;
    }
    else {
      move = -1;
    }
  }
  lastEncoderPos[counter] = encoderpos[counter];
  oldPosition = newPosition;
  if (waiting == 0) {
    checkBattery();
    if (battery_warning) {
      draw_battery_warning();
    }
  }
  if (Serial3.peek() == 'm') {
    Serial3.read();
    Serial3.print('m');
  }

}

void checkBattery() {
  if (Serial3.peek() == 'B') {
    delay(50);
    Serial3.read();
    voltage = read_string();
    current = read_string();
    drawBattery();
  }
}

