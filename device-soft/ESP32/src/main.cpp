#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <drivers/rfidDriver.h>

// Use www.uuidgenerator.net to get new UUIDs

#define BAUD_RATE 115200

#define SERVICE_UUID "975b7600-ced6-4b3a-a4af-be038d43b8c9"

#define FETCH_SIGNAL_CHAR_UUID "f62206df-79d6-4eb9-bfc8-72036bf1e3b3"
#define FETCH_SIGNAL_CHAR_DESC "Client sets this bool 'flag' to call local configuration fetching"

#define RFID_REQUEST_CHAR_UUID "30e27b63-3ee2-486a-a14f-e020be483ada"
#define RFID_REQUEST_CHAR_DESC "Client uses this characteristic to request and recieve the RFID tag data"

#define USER_ID_CHAR_UUID "a4bd67ef-d298-460e-88fd-2857885b0724"
#define USER_ID_CHAR_DESC "ID to get from client when connecting"

#define DEVICE_ID_CHAR_UUID "5a9bec66-0f89-4383-b779-c3b9162d2814"
#define DEVICE_ID_CHAR_DESC "ID to send to client when connecting"

#define SS_PIN 5
#define RST_PIN 0

RfidDriver rfidDriver(SS_PIN, RST_PIN);
RfidDriverManager rm;

BLEServer *pServer = NULL;
BLEService *pService = NULL;

BLECharacteristic *pFetchSignChar = NULL;
BLECharacteristic *pRFIDReqChar = NULL;
BLECharacteristic *pUserIDChar = NULL;
BLECharacteristic *pDeviceIDChar = NULL;

BLEDescriptor *pFetchSignDesc = NULL;
BLEDescriptor *pRFIDReqDesc = NULL;
BLEDescriptor *pUserIDDesc = NULL;
BLEDescriptor *pDeviceIDDesc = NULL;

BLE2902 *pRFIDReqBLE2902 = NULL;
BLE2902 *pDeviceIDBLE2902 = NULL;

static bool connectedClient = false;

static bool fetchRequest = false;
static bool RFIDRequest = false;

const std::string deviceId = "1111-2222-3333";
static std::string userId = "";

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer *pServer) {
        connectedClient = true;
        Serial.println("A client has connected!");

        pDeviceIDChar->setValue(deviceId);
        pDeviceIDChar->notify();
    }

    void onDisconnect(BLEServer *pServer) {
        connectedClient = false;
        Serial.println("The client has disconnected!");

        userId = "";
    }
};

class CharacteristicsCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        if (pCharacteristic == pUserIDChar) {
            userId = pCharacteristic->getValue();
        } else if (pCharacteristic == pFetchSignChar) {
            fetchRequest = true;
        } else if (pCharacteristic == pRFIDReqChar) {
            RFIDRequest = true;
        }
    }
};

void setup() {
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

    pFetchSignChar = pService->createCharacteristic(
        FETCH_SIGNAL_CHAR_UUID,
        BLECharacteristic::PROPERTY_WRITE);

    pRFIDReqChar = pService->createCharacteristic(
        RFID_REQUEST_CHAR_UUID,
        BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_NOTIFY);

    pUserIDChar = pService->createCharacteristic(
        USER_ID_CHAR_UUID,
        BLECharacteristic::PROPERTY_WRITE);

    pDeviceIDChar = pService->createCharacteristic(
        DEVICE_ID_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY);

    // Ass dome info descriptors for all the characteristics

    pFetchSignDesc = new BLEDescriptor((uint16_t)0x2901);
    pFetchSignDesc->setValue(FETCH_SIGNAL_CHAR_DESC);
    pFetchSignChar->addDescriptor(pFetchSignDesc);

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
    configure them with the BLE2902 Descriptor
    */

    pRFIDReqBLE2902 = new BLE2902();
    pRFIDReqBLE2902->setNotifications(true);
    pRFIDReqChar->addDescriptor(pRFIDReqBLE2902);

    pDeviceIDBLE2902 = new BLE2902();
    pDeviceIDBLE2902->setNotifications(true);
    pDeviceIDChar->addDescriptor(pDeviceIDBLE2902);

    pService->start();
    pServer->getAdvertising()->start();

    pFetchSignChar->setCallbacks(new CharacteristicsCallbacks());
    pRFIDReqChar->setCallbacks(new CharacteristicsCallbacks());
    pUserIDChar->setCallbacks(new CharacteristicsCallbacks());

    rm.init(&rfidDriver);
}

void loop() {
    rm.manageUid();
}