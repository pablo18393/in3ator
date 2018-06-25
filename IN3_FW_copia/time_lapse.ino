void time_lapse() {
  page = 4;
  EEPROM.write(0, page);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);
  for (int i = 0; i <= 5; i++) {
    pos_text[i] = 0;
  }
  pos_text[6] = 1;
  if (language) {
    words[0]  = "";
    words[1]  = "";
    words[2]  = "";
    words[3] = "T CLIP";
    words[4] = "INTERVAL";
    words[5] = "T EXPO";
  }
  else {
    words[0]  = "";
    words[1]  = "";
    words[2]  = "";
    words[3] = "CLIP";
    words[4] = "INTERVAL";
    words[5] = "EXP TIME";
  }
  if (keep_am == 0) {
    am_move[0] = 0;
    am_move[1] = 0;
  }
  if (travel_counter || pan_counter) {
    words[6] = "GO";
  }
  else {
    words[6] = "";
  }
  for (byte j = 0; j < 6; j++) {
    tl_data[j] = EEPROM.read(7 + j);
  }
  clip_fps = EEPROM.read(14);
  rectangles = 7;
  drawHeading();
  drawRectangles();
  update_min_interval();
  while (digitalRead(pulse) == 0){
    updateData();
  }
  delay(50);
  selectMode();
}

