#include <Arduino.h>
#include <ArduinoJson.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <ESP32Servo.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <NTPClient.h>
#include <SPI.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include <iomanip>
#include <sstream>

#include "drivers/dispenserDriver.h"
#include "drivers/lcdDriver.h"
#include "drivers/ledDriver.h"
#include "drivers/pickupSys.h"
#include "drivers/rfidDriver.h"

#define BAUD_RATE 9600

#define MAX_SCHEDULE_SIZE_BYTES 4096

#define SERVO_PIN 18

#define LED1_R 16
#define LED1_G 17

#define LED2_R 26
#define LED2_G 27

#define I2C_ADDR 0x27
#define LCD_COLUMNS 16
#define LCD_LINES 2

#define SS_PIN 17
#define RST_PIN 4

#define BLE_DEVICE_NAME "ESP32"

#define WIFI_SSID "Redmi Note 9 Pro"
#define WIFI_PASS "12345768"

#define SCHED_SERV_HOST "dispenser-backend.onrender.com"

char outputBuffer[255];

uint8_t deviceIdBytes[16];
std::string deviceIdBase64 = "";

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);
static LcdDriver ld(&lcd);

Servo motor;
static DispenserDriver dd(&motor);
static DispenceSequence ds(&dd);

static LedManager lm;

Rfid rfid;
static RfidDriver rm;

// Use www.uuidgenerator.net to get new UUIDs

#define SERVICE_UUID "975b7600-ced6-4b3a-a4af-be038d43b8c9"

#define UPDATE_SIGNAL_CHAR_UUID "f62206df-79d6-4eb9-bfc8-72036bf1e3b3"
#define UPDATE_SIGNAL_CHAR_DESC "Client sets this bool 'flag' to call local schedule updating"

#define RFID_REQUEST_CHAR_UUID "30e27b63-3ee2-486a-a14f-e020be483ada"
#define RFID_REQUEST_CHAR_DESC "Client uses this characteristic to request and recieve the RFID tag data"

#define DEVICE_ID_CHAR_UUID "5a9bec66-0f89-4383-b779-c3b9162d2814"
#define DEVICE_ID_CHAR_DESC "ID to send to client when connecting"

StaticJsonDocument<MAX_SCHEDULE_SIZE_BYTES> deviceData;

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
static bool updateRequest = false;

std::string uint8_tToHexString(const uint8_t *, size_t);
std::string uuid4Format(const std::string &);

#define NTP_SERVER_HOST "pool.ntp.org"
#define UTC_OFFSET_SEC 10800

int currTime[2];

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
            updateRequest = true;
        }
        else if (pCharacteristic == pRFIDReqChar)
        {
            // RFID_request = true;
        }
    }
};

void updateSchedule();
void checkSchedule(String);

HTTPClient client;

void setup()
{
    Serial.begin(BAUD_RATE);

    Serial.println("\n\nSetup logs:\n");

    // Device's UUID

    Serial.println("I. Device uuid setup");

    esp_fill_random(deviceIdBytes, sizeof(deviceIdBytes));
    deviceIdBase64 = uuid4Format(uint8_tToHexString(deviceIdBytes, 16));
    deviceIdBase64 = "11111111-1111-1111-1111-111111111111";

    sprintf(outputBuffer, "Device UUID generated: %s\n", deviceIdBase64.c_str());
    Serial.println(outputBuffer);

    // WiFi

    Serial.print("II. ");

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("Connecting to WiFi ");

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }

    sprintf(outputBuffer, "\n--- Successfully connected to %s network\n", WIFI_SSID);
    Serial.println(outputBuffer);

    // BLE

    Serial.println("III. BLE initialization...");

    BLEDevice::init(BLE_DEVICE_NAME);

    sprintf(outputBuffer, "* BLE device intialized - %s", BLE_DEVICE_NAME);
    Serial.println(outputBuffer);

    /*
    The smartphone app initiates the connection,
    so the board is considered to be a server waiting
    for incoming connections
    */

    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    Serial.println("* BLE server created");

    pService = pServer->createService(SERVICE_UUID);

    sprintf(outputBuffer, "* BLE service created: %s", SERVICE_UUID);
    Serial.println(outputBuffer);

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

    rfid.init(SS_PIN, RST_PIN);
    rm.init(&rfid);

    Serial.println("VI. RFID manager initialized successfully\n");

    Serial.println("!Initialization finished!");

    updateRequest = true;
}

void loop()
{
    if (updateRequest)
    {
        updateSchedule();
        updateRequest = false;
    }

    checkSchedule("AQIDBA==");

    // if (ds.checkSeq())
    // {
    //     ds.dispence();
    // }
    // else if (pRFIDReqChar->getValue().size() != 0)
    // {
    //     // if tag is close to rfid reader
    //     if (rm.isNewCardRead())
    //     {
    //         // read uid from tag
    //         rm.readUid();
    //         // check schedule
    //         checkSchedule(rm.getCachedUid());
    //     }
    // }
    // else if (updateRequest)
    // {
    //     updateSchedule();
    //     updateRequest = false;
    // }
    // else if (pRFIDReqChar->getValue().size() == 0)
    // {
    //     // if tag is close to rfid reader
    //     if (rm.isNewCardRead())
    //     {
    //         // read uid from tag
    //         rm.readUid();
    //         // perform some operations...
    //         // pRFIDReqChar->setValue(rm.getCachedUid());
    //     }
    // }

    // ld.update();
    // if (rm.getCachedUid() != "")
    // {
    //     rm.clearCachedUid();
    // }

    delay(5000);
}

void updateSchedule()
{
    if ((WiFi.status() == WL_CONNECTED))
    {
        Serial.println(ESP.getFreeHeap());

        char url[100];
        strcpy(url, "https://");
        strcat(url, SCHED_SERV_HOST);
        strcat(url, "/api/devices/");
        strcat(url, deviceIdBase64.c_str());
        strcat(url, "/config");

        client.begin(url);
        int httpCode = client.GET();

        sprintf(outputBuffer, "GET %s : %d", url, httpCode);
        Serial.println(outputBuffer);

        if (httpCode > 0)
        {
            deviceData.clear();
            DeserializationError error = deserializeJson(deviceData, client.getString());

            if (error)
            {
                Serial.print("deserializeJson() failed: ");
                Serial.println(error.c_str());
                return;
            }

            // Add a bool field to make it possible to ignore already fulfilled schedule items

            JsonArray profiles = deviceData["profiles"];
            for (JsonObject profile : profiles)
            {
                JsonArray pillSchedules = profile["pillSchedules"];
                for (JsonObject pillSchedule : pillSchedules)
                {
                    pillSchedule["dispensedToday"] = false;
                }
            }

            // JsonArray pillSlots = deviceData["pillSlots"];
            // for (JsonObject pillSlot : pillSlots)
            // {
            //     int currPillCount = pillSlot["pillCount"];
            //     pillSlot["pillCount"] = currPillCount + 1;
            // }

            Serial.println("Local schedules successfully updated!");

            serializeJsonPretty(deviceData, Serial);
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

    JsonArray profiles = deviceData["profiles"];
    JsonArray pillSlots = deviceData["pillSlots"];

    for (JsonObject profile : profiles)
    {
        const char *rfid = profile["rfid"];

        if (rfid != nullptr && userRFID.equals(rfid))
        {
            const char *username = profile["username"];

            sprintf(outputBuffer, "Username associated with RFID %s is: %s", userRFID.c_str(), username);
            Serial.println(outputBuffer);

            JsonArray pillSchedules = profile["pillSchedules"];

            for (JsonObject pillSchedule : pillSchedules)
            {
                int hour = pillSchedule["time"]["hour"];
                int minute = pillSchedule["time"]["minutes"];
                int slotNumber = pillSchedule["slotNumber"];

                if (pillSchedule["dispensedToday"])
                {
                    sprintf(outputBuffer, "%d:%d schedule item for %s  was already fulfilled today!", hour, minute, username);
                    Serial.println(outputBuffer);

                    continue;
                }

                if (abs((hour * 60 + minute) - (currTime[0] * 60 + currTime[1])) < 120)
                {
                    JsonObject relevantSlot;

                    for (JsonObject pillSlot : pillSlots)
                    {
                        if (pillSlot["slotNumber"] == slotNumber)
                        {
                            relevantSlot = pillSlot;
                            break;
                        }
                    }

                    if (!relevantSlot)
                    {
                        Serial.println("Slot not found!");
                        continue;
                    }

                    for (int i = 0; i < pillSchedule["quantity"]; i++)
                    {
                        if (relevantSlot["pillCount"] == 0)
                        {
                            sprintf(outputBuffer, "Insufficient supply in slot: %d", slotNumber);
                            Serial.println(outputBuffer);

                            break;
                        }
                        else
                        {
                            ds.pushToSeq(slotNumber);
                            int currPillCount = relevantSlot["pillCount"];
                            relevantSlot["pillCount"] = currPillCount - 1;
                            pillSchedule["dispensedToday"] = true;

                            sprintf(outputBuffer, "%d:%d | slot: %d", hour, minute, slotNumber);
                            Serial.println(outputBuffer);
                        }
                    }

                    lm.update(relevantSlot["pillCount"], slotNumber - 1);
                }
            }
        }

        return;
    }

    serializeJsonPretty(deviceData, Serial);
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
    WiFiUDP ntpUDP;
    NTPClient timeClient(ntpUDP, NTP_SERVER_HOST, UTC_OFFSET_SEC);

    timeClient.begin();
    timeClient.update();
    currTime[0] = timeClient.getHours();
    currTime[1] = timeClient.getMinutes();

    timeClient.end();
}