/*
  MIT License

  Copyright (c) 2022 Medical Open World, Pablo Sánchez Bergasa

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/

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
    digitalWrite(SD_CS, HIGH);
    digitalWrite(TFT_CS, LOW);
  }
  */
}
