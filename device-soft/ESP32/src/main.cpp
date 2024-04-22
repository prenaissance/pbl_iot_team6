#include <Arduino.h>

#include <sstream>
#include <iomanip>

#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

#include <MFRC522.h>
#include <SPI.h>

#include <NTPClient.h>
#include <WiFiUdp.h>

#include <Preferences.h>

#include "drivers/dispenserDriver.h"
#include "drivers/lcdDriver.h"
#include "drivers/ledDriver.h"
#include "drivers/pickupSys.h"
#include "drivers/rfidDriver.h"

#define DEVICE_UUID_NVSK "deviceIdBase64"
#define DEVICE_UUID_SIZE 16

#define BAUD_RATE 9600

#define SERVO_PIN 18

#define LED1_R 16
#define LED1_G 17

#define LED2_R 26
#define LED2_G 27

#define I2C_ADDR 0x27
#define LCD_COLUMNS 16
#define LCD_LINES 2

#define SS_PIN 5
#define RST_PIN 0

const char *ssid = "Redmi Note 9 Pro";
const char *password = "12345768";

const char *scheduleServerHost = "dispenser-backend.onrender.com";

uint8_t deviceIdBytes[DEVICE_UUID_SIZE];
std::string deviceIdBase64 = "";

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);
static LcdDriver ld(&lcd);

Servo motor;
static DispenserDriver dd(&motor);
static DispenceSequence ds(&dd);

static LedManager lm;

Rfid rfid(SS_PIN, RST_PIN);
static RfidDriver rm;

// Use www.uuidgenerator.net to get new UUIDs

#define SERVICE_UUID "975b7600-ced6-4b3a-a4af-be038d43b8c9"

#define UPDATE_SIGNAL_CHAR_UUID "f62206df-79d6-4eb9-bfc8-72036bf1e3b3"
#define UPDATE_SIGNAL_CHAR_DESC "Client sets this bool 'flag' to call local schedule updating"

#define RFID_REQUEST_CHAR_UUID "30e27b63-3ee2-486a-a14f-e020be483ada"
#define RFID_REQUEST_CHAR_DESC "Client uses this characteristic to request and recieve the RFID tag data"

#define DEVICE_ID_CHAR_UUID "5a9bec66-0f89-4383-b779-c3b9162d2814"
#define DEVICE_ID_CHAR_DESC "ID to send to client when connecting"

StaticJsonDocument<1024> userSchedules;

BLEServer *pServer = NULL;
BLEService *pService = NULL;

BLECharacteristic *pUpdateSignChar = NULL;
BLECharacteristic *pRFIDReqChar = NULL;
BLECharacteristic *pDeviceIDChar = NULL;

BLEDescriptor *pUpdateSignDesc = NULL;
BLEDescriptor *pRFIDReqDesc = NULL;
BLEDescriptor *pDeviceIDDesc = NULL;

BLE2902 *pRFIDReqBLE2902 = NULL;
BLE2902 *pDeviceIDBLE2902 = NULL;

static bool connectedClient = false;

static bool UpdateRequest = false;
static bool RFIDRequest = false;

std::string uint8_tToHexString(const uint8_t *, size_t);
std::string uuid4Format(const std::string &);

const char *ntpServer = "pool.ntp.org";
const int utcOffset_sec = 3600 * 3;

int currTime[2];

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, utcOffset_sec);

void getTime();

class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        connectedClient = true;
        Serial.println("A client has connected");

        pDeviceIDChar->setValue(uint8_tToHexString(deviceIdBytes, 16));
    }

    void onDisconnect(BLEServer *pServer)
    {
        connectedClient = false;
        Serial.println("The client has disconnected");
    }
};

class CharacteristicsCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        if (pCharacteristic == pUpdateSignChar)
        {
            UpdateRequest = true;
        }
        else if (pCharacteristic == pRFIDReqChar)
        {
            RFIDRequest = true;
        }
    }
};

void updateSchedule();
void checkSchedule(String);

void setup()
{
    Serial.begin(BAUD_RATE);

    Preferences preferences;
    preferences.begin("device-preferences", false);

    Serial.println("\n\nSetup logs:\n");

    // Device's UUID

    Serial.println("I. Device uuid setup");

    if (preferences.getBytesLength(DEVICE_UUID_NVSK) == DEVICE_UUID_SIZE)
    {
        preferences.getBytes(DEVICE_UUID_NVSK, deviceIdBytes, DEVICE_UUID_SIZE);

        Serial.print("Device UUID - retrieved from NVS: ");
    }
    else
    {
        esp_fill_random(deviceIdBytes, sizeof(deviceIdBytes));
        preferences.putBytes(DEVICE_UUID_NVSK, deviceIdBytes, DEVICE_UUID_SIZE);

        Serial.println("Device UUID - generated: ");
    }

    preferences.end();

    deviceIdBase64 = uuid4Format(uint8_tToHexString(deviceIdBytes, 16));
    deviceIdBase64 = "11111111-1111-1111-1111-111111111111";

    Serial.println(deviceIdBase64.c_str());
    Serial.println();

    // WiFi

    Serial.print("II. ");

    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi ");

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }

    Serial.print("\n--- Successfully connected to ");
    Serial.print(ssid);
    Serial.println(" network\n");

    // BLE

    Serial.println("III. BLE initialization...");

    BLEDevice::init("ESP32");

    Serial.print("* BLE device intialized - ");
    Serial.println("ESP32");

    /*
    The smartphone app initiates the connection,
    so the board is considered to be a server waiting
    for incoming connections
    */

    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    Serial.println("* BLE server created");

    pService = pServer->createService(SERVICE_UUID);

    Serial.print("* BLE service created: ");
    Serial.println(SERVICE_UUID);

    /*
    ~ BLECharacteristic::PROPERTY_WRITE - The characteristic supports writing by the client
    ~ BLECharacteristic::PROPERTY_READ - The characteristic supports reading by the client
    ~ BLECharacteristic::PROPERTY_NOTIFY - The characteristic supports being pushed to the client
    */

    pUpdateSignChar = pService->createCharacteristic(
        UPDATE_SIGNAL_CHAR_UUID,
        BLECharacteristic::PROPERTY_WRITE);

    pRFIDReqChar = pService->createCharacteristic(
        RFID_REQUEST_CHAR_UUID,
        BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_NOTIFY);

    pDeviceIDChar = pService->createCharacteristic(
        DEVICE_ID_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY);

    Serial.println("* Characteristics setup:");
    Serial.print("   ");
    Serial.println(UPDATE_SIGNAL_CHAR_UUID);
    Serial.print("   ");
    Serial.println(RFID_REQUEST_CHAR_UUID);
    Serial.print("   ");
    Serial.println(DEVICE_ID_CHAR_UUID);

    // Add some info descriptors for all the characteristics

    pUpdateSignDesc = new BLEDescriptor((uint16_t)0x2901);
    pUpdateSignDesc->setValue(UPDATE_SIGNAL_CHAR_DESC);
    pUpdateSignChar->addDescriptor(pUpdateSignDesc);

    pRFIDReqDesc = new BLEDescriptor((uint16_t)0x2901);
    pRFIDReqDesc->setValue(RFID_REQUEST_CHAR_DESC);
    pRFIDReqChar->addDescriptor(pRFIDReqDesc);

    pDeviceIDDesc = new BLEDescriptor((uint16_t)0x2901);
    pDeviceIDDesc->setValue(DEVICE_ID_CHAR_DESC);
    pDeviceIDChar->addDescriptor(pDeviceIDDesc);

    Serial.println("* Info descriptors attached:");
    Serial.print("   ");
    Serial.println(UPDATE_SIGNAL_CHAR_DESC);
    Serial.print("   ");
    Serial.println(RFID_REQUEST_CHAR_DESC);
    Serial.print("   ");
    Serial.println(DEVICE_ID_CHAR_DESC);

    /*
    For "notifications" to work properly it is necessary to
    create them with the BLE2902 Descriptor
    */

    pRFIDReqBLE2902 = new BLE2902();
    pRFIDReqBLE2902->setNotifications(true);
    pRFIDReqChar->addDescriptor(pRFIDReqBLE2902);

    pDeviceIDBLE2902 = new BLE2902();
    pDeviceIDBLE2902->setNotifications(true);
    pDeviceIDChar->addDescriptor(pDeviceIDBLE2902);

    Serial.println("* BLE2902 descriptors attached:");
    Serial.print("   ");
    Serial.println(RFID_REQUEST_CHAR_UUID);
    Serial.print("   ");
    Serial.println(DEVICE_ID_CHAR_UUID);

    pUpdateSignChar->setCallbacks(new CharacteristicsCallbacks());
    pRFIDReqChar->setCallbacks(new CharacteristicsCallbacks());

    Serial.println("* Characteristics' callbacks setup");

    pService->start();
    pServer->getAdvertising()->start();

    Serial.println("--- Service & server advertisement started successfully\n");

    motor.attach(SERVO_PIN);

    Serial.println("IV. Motor pinout set up successfully\n");

    lm.init(LedDriver(LED1_R, LED1_G), 0);
    lm.init(LedDriver(LED2_R, LED2_G), 1);

    Serial.println("V. LED indication system initialized successfully\n");

    rm.init(&rfid);

    Serial.println("VI. RFID manager initialized successfully\n");

    timeClient.begin();

    Serial.println("VII. Time client started successfully\n");

    Serial.println("!Initialization finished!");

    updateSchedule();
}

void loop()
{
    checkSchedule("AQIDBA==");

    delay(10000);

    // // rm.manageUid();
}

void updateSchedule()
{
    if ((WiFi.status() == WL_CONNECTED))
    {
        HTTPClient client;

        char url[100] = "https://";
        strcat(url, scheduleServerHost);
        strcat(url, "/api/devices/");
        strcat(url, deviceIdBase64.c_str());
        strcat(url, "/config");

        client.begin(url);
        int httpCode = client.GET();

        if (httpCode > 0)
        {
            String payload = client.getString();
            userSchedules.clear();
            DeserializationError error = deserializeJson(userSchedules, payload);

            if (error)
            {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.c_str());
                return;
            }

            // Add a bool field to make it possible to ignore already fulfilled schedule items

            JsonArray profiles = userSchedules["profiles"];
            for (JsonObject profile : profiles)
            {
                JsonArray pillSchedules = profile["pillSchedules"];
                for (JsonObject pillSchedule : pillSchedules)
                {
                    pillSchedule["dispensedToday"] = false;
                }
            }

            Serial.println("Local schedule successfully updated!");
        }
        else
        {
            Serial.println("Error on HTTP request");
        }

        client.end();
    }
    else
    {
        Serial.println("No WiFi connection!");
    }
}

void checkSchedule(String userRFID)
{
    getTime();

    JsonArray profiles = userSchedules["profiles"];
    for (JsonObject profile : profiles)
    {
        const char *rfid = profile["rfid"];

        if (rfid != nullptr && userRFID.equals(rfid))
        {
            const char *username = profile["username"];

            Serial.print("Username associated with RFID ");
            Serial.print(userRFID);
            Serial.print(" is: ");
            Serial.println(username);

            JsonArray pillSchedules = profile["pillSchedules"];
            for (JsonObject pillSchedule : pillSchedules)
            {
                int hour = pillSchedule["time"]["hour"];
                int minute = pillSchedule["time"]["minutes"];

                if (pillSchedule["dispensedToday"])
                {
                    Serial.println(String(hour) + ':' + String(minute) + " schedule item for " + username + " was already fulfilled for today!");
                    continue;
                }

                int slotNumber = pillSchedule["slotNumber"];

                if (abs((hour * 60 + minute) - (currTime[0] * 60 + currTime[1])) < 30)
                {
                    int quantity = pillSchedule["quantity"];

                    if (quantity == 0)
                    {
                        Serial.println("Insufficient supply in slot: " + String(slotNumber));
                        continue;
                    }

                    Serial.println("Dispence:");
                    Serial.println(String(hour) + ':' + String(minute) + " | slot: " + String(slotNumber));

                    ds.pushToSeq(slotNumber);
                    pillSchedule["dispensedToday"] = true;

                    for (JsonObject updPillSchedule : pillSchedules)
                    {
                        int updItemSlotNumber = updPillSchedule["slotNumber"];
                        if (updItemSlotNumber == slotNumber)
                        {
                            int currQuan = updPillSchedule["quantity"];
                            updPillSchedule["quantity"] = currQuan - 1;
                        }
                    }

                    lm.update(pillSchedule["quantity"], slotNumber - 1);
                }
            }
        }

        serializeJsonPretty(userSchedules, Serial);

        return;
    }
}

std::string uint8_tToHexString(const uint8_t *data, size_t len)
{
    std::stringstream ss;
    ss << std::hex;

    for (size_t i = 0; i < len; ++i)
    {
        ss << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]);
    }

    return ss.str();
}

std::string uuid4Format(const std::string &uuid_str)
{
    std::string formatted_uuid =
        uuid_str.substr(0, 8) + "-" +
        uuid_str.substr(8, 4) + "-" +
        uuid_str.substr(12, 4) + "-" +
        uuid_str.substr(16, 4) + "-" +
        uuid_str.substr(20);

    return formatted_uuid;
}

void getTime()
{
    timeClient.update();
    currTime[0] = timeClient.getHours();
    currTime[1] = timeClient.getMinutes();
}