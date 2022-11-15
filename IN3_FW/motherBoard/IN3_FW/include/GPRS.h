//#define TOKEN "tOUZTeBHHycW5BGntTuO"
#define THINGSBOARD_SERVER "mon.medicalopenworld.org"
#define THINGSBOARD_PORT 1883

// Baud rate for debug serial
#define SERIAL_DEBUG_BAUD 115200
#define MODEM_BAUD 115200
#define RX_BUFFER_LENGTH 1024
#define GPRS_TIMEOUT 60000 // in millisecs

#define modemSerial Serial2

#define standByGPRSPostPeriod 30
#define actuatingGPRSPostPeriod 60
#define phototherapyGPRSPostPeriod 180
#define GPRS_SHUT OFF

#define APN_TM "TM"
#define APN_TRUPHONE "iot.truphone.com"

#define GPRS_USER ""
#define GPRS_PASS ""

#define PROVISION_DEVICE_KEY "3ze2np7my4acf643r0jz"
#define PROVISION_DEVICE_SECRET "nruedye0eiz1dkm8mo96"

#define SIMCOM800_AT "AT\n"
#define SIMCOM800_AT_CFUN "AT+CFUN=1\n"

#define AT_OK "OK"
#define AT_ERROR "ERROR"

struct GPRSstruct
{
    int provisioned = false;
    bool provision_request_sent = false;
    bool provision_request_processed = false;

    bool enable;
    long sendPeriod;
    long lastSent;
    char buffer[RX_BUFFER_LENGTH];
    int charToRead;
    int bufferWritePos;
    bool powerUp;
    bool connect;
    bool connectionStatus;
    bool timeOut;
    byte process;
    long processTime;
    long packetSentenceTime;
    bool post;
    bool firstPost = false;
    bool firstConfigPost;

    String CCID;
    String IMEI;
    String IMSI;
    String COP;
    int CSQ;
    String APN;
    IPAddress IP;
    String device_token;

    float longitud;
    float latitud;
    float accuracy;
};

// Struct for client connecting after provisioning
struct Credentials {
  String client_id;
  String username;
  String password;
};