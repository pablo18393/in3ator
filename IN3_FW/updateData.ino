int updateData() {
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
  if (!page) {
    if (millis() - last_temp_update > temp_update_rate) {
      last_temp_update = millis();
      updateTemperature();
    }
  }
  return move;
}

void checkBattery() {

}

