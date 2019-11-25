String logFile = "datalog.txt";
String GSMFile = "GSM.txt";

void initSD() {
  SD.begin(SD_CS);
  if (!SD.exists(GSMFile)) {
    logln("Creating log file");
    File dataFile = SD.open(GSMFile, FILE_WRITE);
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
    dataFile.print(dataString);
    dataFile.close();
    // print to the serial port too:
  }
  Serial.print(dataString);
  digitalWrite(SD_CS, HIGH);
  digitalWrite(TFT_CS, LOW);
}

void SDlog() {
  digitalWrite(SD_CS, LOW);
  digitalWrite(TFT_CS, HIGH);
  String dataString;
  dataString += temperature[babyNTC];
  dataString += ",";
  dataString += humidity;
  dataString += ",";
  // if the file is available, write to it:
  File dataFile = SD.open(GSMFile, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    logln(dataString);
  }
  digitalWrite(SD_CS, HIGH);
  digitalWrite(TFT_CS, LOW);
}

void updatedTimeStamp(){
  
}
