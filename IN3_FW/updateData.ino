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
  if (millis() - last_temp_update > temp_update_rate) {
    last_temp_update = millis();
    updateTemperature();
    temperature_measured = 0;
  }
  else {

  }
  //PROBLEM WITH THIS FUNCTION: temperature measure time is not constant
  if (millis() - last_temp_update > (temp_update_rate * temperature_measured / temperature_fraction)) {
    temperatureArray[temperature_measured]=analogRead(THERMISTOR1);
    temperature_measured++;
  }
  return move;
}

void checkBattery() {

}

