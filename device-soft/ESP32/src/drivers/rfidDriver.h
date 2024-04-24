#include <MFRC522.h>
#include <SPI.h>

#define MAX_KEY_SIZE 6

class Rfid {
   private:
    MFRC522 *rfid;
    MFRC522::MIFARE_Key key;

   public:
    void init(int ssPin, int rstPin) {
        rfid = new MFRC522(ssPin, rstPin);
        SPI.begin();       // Init SPI bus
        rfid->PCD_Init();  // Init MFRC522

        for (byte i = 0; i < MAX_KEY_SIZE; ++i) {
            key.keyByte[i] = 0xFF;
        }
    }

    MFRC522 *getRfid() { return rfid; }
};

class RfidDriver {
   private:
    Rfid *instance;
    String chachedUid;

    String convertHexToString(byte *buffer, byte bufferSize) {
        String output = "";

        for (byte i = 0; i < bufferSize; ++i) {
            char hexBuff[3];
            sprintf(hexBuff, "%02X", buffer[i]);
            output += hexBuff;
        }

        return output;
    }

   public:
    void init(Rfid *instance) { this->instance = instance; }

    bool isNewCardRead() {
        MFRC522 *rfid = instance->getRfid();
        return rfid->PICC_IsNewCardPresent() && rfid->PICC_ReadCardSerial();
    }

    void readUid() {
        MFRC522 *rfid = instance->getRfid();

        // Record UID
        chachedUid = convertHexToString(rfid->uid.uidByte, rfid->uid.size);

        // Halt PICC
        rfid->PICC_HaltA();

        // Stop encryption on PCD
        rfid->PCD_StopCrypto1();
    }

    String getCachedUid() {
        return chachedUid;
    }

    void clearCachedUid() {
        chachedUid = "";
    }
};