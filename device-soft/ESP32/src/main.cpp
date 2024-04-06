#include <Arduino.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define BAUD_RATE 115200

#define SERVICE_UUID        "975b7600-ced6-4b3a-a4af-be038d43b8c9"

#define FETCH_CHAR_UUID     "f62206df-79d6-4eb9-bfc8-72036bf1e3b3"
#define RFID_CHAR_UUID      "63040f29-4670-4e67-925b-53b8b05cff60"
#define USER_ID_CHAR_UUID   "8c52f345-9a0a-415e-89d7-534089165e62"
#define DEVICE_ID_CHAR_UUID "f6934998-48e2-4cc5-9584-de4f2d2498b0"

BLECharacteristic *pFetchChar = NULL;
BLECharacteristic *pRFIDChar = NULL;
BLECharacteristic *pUIDChar = NULL;
BLECharacteristic *pDIDChar = NULL;

static bool connected = false;

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEClient* pclient) {
        connected = true;
        Serial.println("Connected");
    }

    void onDisconnect(BLEClient* pclient) {
        connected = false;
        Serial.println("Disconnected");
    }
};

void setup(){
    Serial.begin(BAUD_RATE);

    BLEDevice::init("ESP32");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService *pService = pServer->createService(SERVICE_UUID);

    pFetchChar = pService->createCharacteristic(
        FETCH_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ    |
        BLECharacteristic::PROPERTY_WRITE   |
        BLECharacteristic::PROPERTY_NOTIFY
    );

    pRFIDChar = pService->createCharacteristic(
        RFID_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ    |
        BLECharacteristic::PROPERTY_WRITE   |
        BLECharacteristic::PROPERTY_NOTIFY
    );

    pUIDChar = pService->createCharacteristic(
        USER_ID_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ    |
        BLECharacteristic::PROPERTY_WRITE   |
        BLECharacteristic::PROPERTY_NOTIFY
    );

    pDIDChar = pService->createCharacteristic(
        DEVICE_ID_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ    |
        BLECharacteristic::PROPERTY_WRITE   |
        BLECharacteristic::PROPERTY_NOTIFY
    );

    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->start();
}

void loop(){

}