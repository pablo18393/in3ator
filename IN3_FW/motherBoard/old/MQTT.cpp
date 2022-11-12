
#include "MQTT.h"

StreamDebugger debugger(Serial2, Serial);

// See https://thingsboard.io/docs/getting-started-guides/helloworld/
// to understand how to obtain an access token
#define TOKEN "tOUZTeBHHycW5BGntTuO"
#define THINGSBOARD_SERVER "mon.medicalopenworld.org"

// Baud rate for debug serial
#define SERIAL_DEBUG_BAUD 115200
#define MODEM_BAUD 115200

#define GPRS_CONNECTION_TIMEOUT 2000

#define modemSerial Serial2
#define DEFAULT_POST_PERIOD 10

// Initialize GSM modem
TinyGsm modem(modemSerial);

// Initialize GSM client
TinyGsmClient client(modem);

// Initialize ThingsBoard instance
ThingsBoard tb(client);

// Set to true, if modem is connected
bool modemConnected = false;
bool modemPowered = false;
long postPeriod, lastPostPeriod = false;

bool modemCheckPowered()
{
    String ccid = modem.getSimCCID();
    Serial.println("CCID:" + ccid);
    if (ccid.length() > 0)
    {
        Serial.println(ccid.length());
        modemPowered = true;
        return (true);
    }
    return (false);
}

void powerSIM800C()
{
    logln("Powering modem");
    pinMode(GPRS_PWRKEY, OUTPUT);
    digitalWrite(GPRS_PWRKEY, HIGH);
    delay(2000);
    digitalWrite(GPRS_PWRKEY, LOW);
    delay(2000);
    digitalWrite(GPRS_PWRKEY, HIGH);
}

// in seconds, how often post values
void MQTT_SetPostPeriod(long value)
{
    postPeriod = value;
}

void MQTT_init()
{
    // Set GSM module baud rate
    modemSerial.begin(MODEM_BAUD);
    logln(F("Initializing modem..."));
    if (!modemCheckPowered())
    {
        powerSIM800C();
    }
    else
    {
        logln("modem already powered");
    }
    if (!modemCheckPowered())
    {
        logln("Modem power FAIL");
        return;
    }
    String modemInfo = modem.getModemInfo();
    logln("Modem: " + modemInfo);

    if (!modemConnected)
    {
        Serial.print(F("Waiting for network..."));
        if (!modem.waitForNetwork(GPRS_CONNECTION_TIMEOUT))
        {
            Serial.println(" fail");
            return;
        }
        Serial.println(" OK");

        Serial.print(F("Connecting to "));
        Serial.print(apn);
        if (!modem.gprsConnect(apn, user, pass))
        {
            Serial.print(F("Connecting to alternative APN"));
            Serial.print(apn2);
            if (!modem.gprsConnect(apn2, user, pass))
            {
                return;
            }
        }
        String cop = modem.getOperator();
        DBG("Operator:", cop);
        int csq = modem.getSignalQuality();
        DBG("Signal quality:", csq);

        float lat = 0;
        float lon = 0;
        float accuracy = 0;
        int year = 0;
        int month = 0;
        int day = 0;
        int hour = 0;
        int min = 0;
        int sec = 0;
        DBG("Requesting current GSM location");
        if (modem.getGsmLocation(&lat, &lon, &accuracy, &year, &month, &day, &hour,
                                 &min, &sec))
        {
            DBG("Latitude:", String(lat, 8), "\tLongitude:", String(lon, 8));
            DBG("Accuracy:", accuracy);
            DBG("Year:", year, "\tMonth:", month, "\tDay:", day);
            DBG("Hour:", hour, "\tMinute:", min, "\tSecond:", sec);
        }
        modemConnected = true;
        Serial.println("GPRS Connected");
        MQTT_SetPostPeriod(DEFAULT_POST_PERIOD);
    }
}

void MQTT_Post()
{
    if (postPeriod && millis() - lastPostPeriod > millisToSecs(postPeriod))
    {

    }
}