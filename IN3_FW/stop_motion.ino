void stop_motion(){
    page = 6;
  EEPROM.write(0, page);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);
  for (int i = 0; i <= 5; i++) {
    pos_text[i] = 0;
  }
  pos_text[4] = 1;
  if (language) {
    words[0]  = "";
    words[1]  = "";
    words[2]  = "";
    words[3] = "T CLIP";
  }
  else {
    words[0]  = "";
    words[1]  = "";
    words[2]  = "";
    words[3] = "T CLIP";
  }
  if (keep_am == 0) {
    am_move[0] = 0;
    am_move[1] = 0;
  }
  if (travel_counter || pan_counter) {
    words[4] = "GO";
  }
  else {
    words[4] = "";
  }
  for (byte j = 0; j < 6; j++) {
    tl_data[j] = EEPROM.read(7 + j);
  }
  rectangles = 5;
  drawHeading();
  drawRectangles();
  while (digitalRead(pulse) == 0){
    updateData();
  }
  delay(50);
  selectMode();
}

