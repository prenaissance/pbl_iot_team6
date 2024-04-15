#include <Arduino.h>
#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "drivers/dispenserDriver.h"
#include "drivers/lcdDriver.h"
#include "drivers/pickupSys.h"
#include "drivers/rfidDriver.h"
#include "drivers/ledDriver.h"

#define BAUD_RATE 9600

#define SERVO_PIN 18

#define LED1_R 16
#define LED1_G 17

#define LED2_R 26
#define LED2_G 27

#define I2C_ADDR 0x27
#define LCD_COLUMNS 16
#define LCD_LINES 2

const char* ssid = "Redmi Note 9 Pro";
const char* password = "12345768";

const char* scheduleServerHost = "";
const char* scheduleUpdateEp = "";

std::string deviceId = "1111-2222-3333";
std::string userId = "";

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);
static LcdDriver ld(&lcd);

Servo motor;
static DispenserDriver dd(&motor);

static LedManager lm;

// Use www.uuidgenerator.net to get new UUIDs

#define SERVICE_UUID            "975b7600-ced6-4b3a-a4af-be038d43b8c9"

#define UPDATE_SIGNAL_CHAR_UUID  "f62206df-79d6-4eb9-bfc8-72036bf1e3b3"
#define UPDATE_SIGNAL_CHAR_DESC  "Client sets this bool 'flag' to call local schedule Updateing"

#define RFID_REQUEST_CHAR_UUID  "30e27b63-3ee2-486a-a14f-e020be483ada"
#define RFID_REQUEST_CHAR_DESC  "Client uses this characteristic to request and recieve the RFID tag data"

#define USER_ID_CHAR_UUID       "a4bd67ef-d298-460e-88fd-2857885b0724"
#define USER_ID_CHAR_DESC       "ID to get from client when connecting"

#define DEVICE_ID_CHAR_UUID     "5a9bec66-0f89-4383-b779-c3b9162d2814"
#define DEVICE_ID_CHAR_DESC     "ID to send to client when connecting"

StaticJsonDocument<1024> schedule;

BLEServer *pServer = NULL;
BLEService *pService = NULL;

BLECharacteristic *pUpdateSignChar = NULL;
BLECharacteristic *pRFIDReqChar = NULL;
BLECharacteristic *pUserIDChar = NULL;
BLECharacteristic *pDeviceIDChar = NULL;

BLEDescriptor *pUpdateSignDesc = NULL;
BLEDescriptor *pRFIDReqDesc = NULL;
BLEDescriptor *pUserIDDesc = NULL;
BLEDescriptor *pDeviceIDDesc = NULL;

BLE2902 *pRFIDReqBLE2902 = NULL;
BLE2902 *pDeviceIDBLE2902 = NULL;

static bool connectedClient = false;

static bool UpdateRequest = false;
static bool RFIDRequest = false;

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        connectedClient = true;
        Serial.println("A client has connected!");

        pDeviceIDChar->setValue(deviceId);
        pDeviceIDChar->notify();
    }

    void onDisconnect(BLEServer* pServer) {
        connectedClient = false;
        Serial.println("The client has disconnected!");

        userId = "";
    }
};

class CharacteristicsCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        if (pCharacteristic == pUserIDChar) {
            userId = pCharacteristic->getValue();
        } else if (pCharacteristic == pUpdateSignChar) {
            UpdateRequest = true;
        } else if (pCharacteristic == pRFIDReqChar) {
            RFIDRequest = true;
        }
    }
};

void updateSchedule();

void setup(){
    Serial.begin(BAUD_RATE);

    BLEDevice::init("ESP32");

    /* 
    The smartphone app initiates the connection, 
    so the board is considered to be a server waiting 
    for incoming connections
    */

    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    pService = pServer->createService(SERVICE_UUID);

    /*
    ~ BLECharacteristic::PROPERTY_WRITE - The characteristic supports writing by the client
    ~ BLECharacteristic::PROPERTY_READ - The characteristic supports reading by the client
    ~ BLECharacteristic::PROPERTY_NOTIFY - The characteristic supports being pushed to the client
    */

    pUpdateSignChar = pService->createCharacteristic(
        UPDATE_SIGNAL_CHAR_UUID,
        BLECharacteristic::PROPERTY_WRITE
    );

    pRFIDReqChar = pService->createCharacteristic(
        RFID_REQUEST_CHAR_UUID,
        BLECharacteristic::PROPERTY_WRITE   |
        BLECharacteristic::PROPERTY_NOTIFY
    );

    pUserIDChar = pService->createCharacteristic(
        USER_ID_CHAR_UUID,
        BLECharacteristic::PROPERTY_WRITE
    );

    pDeviceIDChar = pService->createCharacteristic(
        DEVICE_ID_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_NOTIFY
    );

    // Ass dome info descriptors for all the characteristics

    pUpdateSignDesc = new BLEDescriptor((uint16_t)0x2901);
    pUpdateSignDesc->setValue(UPDATE_SIGNAL_CHAR_DESC);
    pUpdateSignChar->addDescriptor(pUpdateSignDesc);

    pRFIDReqDesc = new BLEDescriptor((uint16_t)0x2901);
    pRFIDReqDesc->setValue(RFID_REQUEST_CHAR_DESC);
    pRFIDReqChar->addDescriptor(pRFIDReqDesc);

    pUserIDDesc = new BLEDescriptor((uint16_t)0x2901);
    pUserIDDesc->setValue(USER_ID_CHAR_DESC);
    pUserIDChar->addDescriptor(pUserIDDesc);

    pDeviceIDDesc = new BLEDescriptor((uint16_t)0x2901);
    pDeviceIDDesc->setValue(DEVICE_ID_CHAR_DESC);
    pDeviceIDChar->addDescriptor(pDeviceIDDesc);

    /*
    For "notifications" to work properly it is necessary to 
    scheduleure them with the BLE2902 Descriptor
    */

    pRFIDReqBLE2902 = new BLE2902();
    pRFIDReqBLE2902->setNotifications(true);
    pRFIDReqChar->addDescriptor(pRFIDReqBLE2902);

    pDeviceIDBLE2902 = new BLE2902();
    pDeviceIDBLE2902->setNotifications(true);
    pDeviceIDChar->addDescriptor(pDeviceIDBLE2902);

    pService->start();
    pServer->getAdvertising()->start();

    pUpdateSignChar->setCallbacks(new CharacteristicsCallbacks());
    pRFIDReqChar->setCallbacks(new CharacteristicsCallbacks());
    pUserIDChar->setCallbacks(new CharacteristicsCallbacks());

    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    motor.attach(SERVO_PIN);

    lm.init(LedDriver(LED1_R, LED1_G), 0);
    lm.init(LedDriver(LED2_R, LED2_G), 1);
}

void loop() {
}

void updateSchedule() {
    if ((WiFi.status() == WL_CONNECTED)) {
        HTTPClient client;

        char url[64];
        strcpy(url, scheduleServerHost);
        strcat(url, scheduleUpdateEp);
        strcat(url, deviceId.c_str());

        client.begin(url);
        int httpCode = client.GET();

        if (httpCode > 0) {
            String payload = client.getString();
            Serial.println("\nStatuscode: " + String(httpCode));
            Serial.println(payload);

            DeserializationError error = deserializeJson(schedule, payload);

            if (error) {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.c_str());
                return;
            }

            Serial.println("Local schedule successfully updated!");
        } else {
            Serial.println("Error on HTTP request");
        }
    } else {
        Serial.println("No WiFi connection!");
    }
}