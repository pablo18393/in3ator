String logFile = "datalog.txt";
String GPRSFile = "GPRS.txt";

void initSD() {
  SPI.setModule(SPI_SEL);
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
  digitalWrite(SD_CS, HIGH);
  digitalWrite(TFT_CS, LOW);
}

void logln(String dataString) {
  log(dataString + '\n');
}

void log(String dataString) {
  digitalWrite(SD_CS, LOW);
  digitalWrite(TFT_CS, HIGH);
  // if the file is available, write to it:
  File dataFile = SD.open(logFile, FILE_WRITE);
  if (dataFile) {
    dataFile.print(day());
    dataFile.print("/");
    dataFile.print(month());
    dataFile.print("/");
    dataFile.print(year());
    dataFile.print("-");
    dataFile.print(hour());
    dataFile.print(":");
    dataFile.print(minute());
    dataFile.print(":");
    dataFile.print(second());
    dataFile.print(";");
    dataFile.print(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  Serial.print(dataString);
  digitalWrite(SD_CS, HIGH);
  digitalWrite(TFT_CS, LOW);
}

void GPRSLocalLog() {
  digitalWrite(SD_CS, LOW);
  digitalWrite(TFT_CS, HIGH);
  String dataString;
  dataString += temperature[babyNTC];
  dataString += ";";
  dataString += humidity;
  dataString += ";";
  // if the file is available, write to it:
  File dataFile = SD.open(GPRSFile, FILE_WRITE);
  if (dataFile) {
    dataFile.print(day());
    dataFile.print("/");
    dataFile.print(month());
    dataFile.print("/");
    dataFile.print(year());
    dataFile.print("-");
    dataFile.print(hour());
    dataFile.print(":");
    dataFile.print(minute());
    dataFile.print(":");
    dataFile.print(second());
    dataFile.print(";");
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    //logln(dataString);
  }
  digitalWrite(SD_CS, HIGH);
  digitalWrite(TFT_CS, LOW);
}

void updatedTimeStamp() {

}
