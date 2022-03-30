String logFile = "datalog.txt";
String GPRSFile = "GPRS.txt";

void initSD() {
  /*
  if (SDCard) {
    SD.begin(SD_CS);
    if (!SD.exists(GPRSFile)) {
      logln("Creating log file");
      File dataFile = SD.open(GPRSFile, FILE_WRITE);
      //String dataString = "SN, babyTemp,Humidity, HeaterTemp, BoardTemp1, BoardTemp2, BoardTemp3, Longitud, Latitud, JaudicePower, BPM, IBI, RPS, HeaterPower";
      String dataString = "babyTemp,Humidity";
      // if the file is available, write to it:
      if (dataFile) {
        dataFile.println(dataString);
        dataFile.close();
        // print to the serial port too:
        logln(dataString);
      }
    }
    GPIOWrite(SD_CS, HIGH);
    GPIOWrite(TFT_CS, LOW);
  }
  */
}
