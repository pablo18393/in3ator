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
#include <Arduino.h>
#include "main.h"

const char *wifiHost = "in3ator";

const char *ssid = "in3wifi";
const char *wifiPassword = "12345678";

WebServer wifiServer(80);

WiFiClient espClient;

// Initialize ThingsBoard instance
ThingsBoardSized<THINGSBOARD_BUFFER_SIZE> tb_wifi(espClient);

// WIFI
bool WIFI_connection_status = false;

extern in3ator_parameters in3;
WIFIstruct Wifi_TB;

/*
   Login page
*/

const char *loginIndex =
    "<form name='loginForm'>"
    "<table width='20%' bgcolor='A09F9F' align='center'>"
    "<tr>"
    "<td colspan=2>"
    "<center><font size=4><b>ESP32 Login Page</b></font></center>"
    "<br>"
    "</td>"
    "<br>"
    "<br>"
    "</tr>"
    "<tr>"
    "<td>Username:</td>"
    "<td><input type='text' size=25 name='userid'><br></td>"
    "</tr>"
    "<br>"
    "<br>"
    "<tr>"
    "<td>Password:</td>"
    "<td><input type='Password' size=25 name='pwd'><br></td>"
    "<br>"
    "<br>"
    "</tr>"
    "<tr>"
    "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
    "</tr>"
    "</table>"
    "</form>"
    "<script>"
    "function check(form)"
    "{"
    "if(form.userid.value=='in3admin' && form.pwd.value=='savinglives')"
    "{"
    "window.open('/serverIndex')"
    "}"
    "else"
    "{"
    " alert('Error Password or Username')/*displays error message*/"
    "}"
    "}"
    "</script>";

/*
   wifiServer Index Page
*/

const char *serverIndex =
    "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
    "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
    "<input type='file' name='update'>"
    "<input type='submit' value='Update'>"
    "</form>"
    "<div id='prg'>progress: 0%</div>"
    "<script>"
    "$('form').submit(function(e){"
    "e.preventDefault();"
    "var form = $('#upload_form')[0];"
    "var data = new FormData(form);"
    " $.ajax({"
    "url: '/update',"
    "type: 'POST',"
    "data: data,"
    "contentType: false,"
    "processData:false,"
    "xhr: function() {"
    "var xhr = new window.XMLHttpRequest();"
    "xhr.upload.addEventListener('progress', function(evt) {"
    "if (evt.lengthComputable) {"
    "var per = evt.loaded / evt.total;"
    "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
    "}"
    "}, false);"
    "return xhr;"
    "},"
    "success:function(d, s) {"
    "console.log('success!')"
    "},"
    "error: function (a, b, c) {"
    "}"
    "});"
    "});"
    "</script>";

/*
   setup function
*/
void wifiInit(void)
{
  // Connect to WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(String(String(WIFI_NAME) + "_" + String(in3.serialNumber)).c_str());
  WiFi.begin(ssid, wifiPassword);
}

void wifiDisable()
{
  WiFi.mode(WIFI_OFF);
}

void configWifiServer()
{
  // Wait for connection
  logln("Connected to " + String(ssid) + "IP address" + WiFi.localIP());

  /*use mdns for wifiHost name resolution*/
  if (!MDNS.begin(wifiHost))
  { // http://esp32.local
    logln("Error setting up MDNS responder!");
  }
  logln("mDNS responder started");
  /*return index page which is stored in ServerIndex */
  wifiServer.on("/", HTTP_GET, []()
                {
    wifiServer.sendHeader("Connection", "close");
    wifiServer.send(200, "text/html", loginIndex); });
  wifiServer.on("/serverIndex", HTTP_GET, []()
                {
    wifiServer.sendHeader("Connection", "close");
    wifiServer.send(200, "text/html", serverIndex); });
  /*handling uploading firmware file */
  wifiServer.on(
      "/update", HTTP_POST, []()
      {
    wifiServer.sendHeader("Connection", "close");
    wifiServer.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart(); },
      []()
      {
        HTTPUpload &upload = wifiServer.upload();
        if (upload.status == UPLOAD_FILE_START)
        {
          Serial.printf("Update: %s\n", upload.filename.c_str());
          if (!Update.begin(UPDATE_SIZE_UNKNOWN))
          { // start with max available size
            Update.printError(Serial);
          }
        }
        else if (upload.status == UPLOAD_FILE_WRITE)
        {
          /* flashing firmware to ESP*/
          if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
          {
            Update.printError(Serial);
          }
        }
        else if (upload.status == UPLOAD_FILE_END)
        {
          if (Update.end(true))
          { // true to set the size to the current progress
            Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
          }
          else
          {
            Update.printError(Serial);
          }
        }
      });
  wifiServer.begin();
}

void WIFI_UpdatedCallback(const bool &success)
{
  if (success)
  {
    logln("[WIFI] -> Done, Reboot now");
    esp_restart();
  }
  else
  {
    logln("[WIFI] -> No new firmware");
  }
}

bool WIFICheckNewEvent()
{
  bool retVal = false;
  bool WifiStatus = (WiFi.status() == WL_CONNECTED);
  bool serverConnectionStatus = WIFIIsConnectedToServer();
  bool OTAInProgress = WIFIOTAIsOngoing();
  if (serverConnectionStatus != Wifi_TB.lastServerConnectionStatus || WifiStatus != Wifi_TB.lastWIFIConnectionStatus || OTAInProgress != Wifi_TB.lastOTAInProgress)
  {
    retVal = true;
  }
  Wifi_TB.lastOTAInProgress = OTAInProgress;
  Wifi_TB.lastWIFIConnectionStatus = WifiStatus;
  Wifi_TB.lastServerConnectionStatus = serverConnectionStatus;
  return (retVal);
}

bool WIFIIsConnected()
{
  return (WiFi.status() == WL_CONNECTED);
}

bool WIFIIsConnectedToServer()
{
  return (Wifi_TB.serverConnectionStatus && WIFIIsConnected());
}

bool WIFIOTAIsOngoing()
{
  return (tb_wifi.Firmware_is_updating() && WIFIIsConnectedToServer());
}

void WIFICheckOTA()
{
  logln("[WIFI] -> Checking WIFI firwmare Update...");
  tb_wifi.Start_Firmware_Update(CURRENT_FIRMWARE_TITLE, FWversion, WIFI_UpdatedCallback);
}

void WIFI_TB_Init()
{
  Wifi_TB.provisioned = EEPROM.read(EEPROM_THINGSBOARD_PROVISIONED);
  if (Wifi_TB.provisioned)
  {
    Wifi_TB.device_token = EEPROM.readString(EEPROM_THINGSBOARD_TOKEN);
  }
}

void WifiOTAHandler(void)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    if (!WIFI_connection_status)
    {
      configWifiServer();
    }
    else
    {
      wifiServer.handleClient();
    }
    if (!tb_wifi.connected())
    {
      // Connect to the ThingsBoard
      logln("[WIFI] -> Connecting over WIFI to: " + String(THINGSBOARD_SERVER) + " with token " + String(Wifi_TB.device_token));
      if (!tb_wifi.connect(THINGSBOARD_SERVER, Wifi_TB.device_token.c_str()))
      {
        logln("[WIFI] ->Failed to connect");
        return;
      }
      else
      {
        Wifi_TB.serverConnectionStatus = true;
        if (ENABLE_WIFI_OTA && !Wifi_TB.OTA_requested)
        {
          WIFICheckOTA();
          Wifi_TB.OTA_requested = true;
        }
      }
    }
    else
    {
      tb_wifi.loop();
    }
    WIFI_connection_status = true;
  }
  else
  {
    WIFI_connection_status = false;
    Wifi_TB.serverConnectionStatus = false;
  }
}
