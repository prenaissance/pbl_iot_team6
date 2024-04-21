#include <MFRC522.h>
#include <SPI.h>

#define MAX_KEY_SIZE 6

class Rfid
{
private:
    MFRC522 rfid;
    MFRC522::MIFARE_Key key;

public:
    Rfid() : rfid(0, 0) {}

    Rfid(int ssPin, int rstPin) : rfid(ssPin, rstPin)
    {
        SPI.begin();     // Init SPI bus
        rfid.PCD_Init(); // Init MFRC522

        for (byte i = 0; i < MAX_KEY_SIZE; ++i)
        {
            key.keyByte[i] = 0xFF;
        }
    }

    MFRC522 getRfid() { return rfid; }
};

class RfidDriver
{
private:
    Rfid *instance;

    String convertHexToString(byte *buffer, byte bufferSize)
    {
        String output = "";

        for (byte i = 0; i < bufferSize; ++i)
        {
            char hexBuff[3];
            sprintf(hexBuff, "%02X", buffer[i]);
            output += hexBuff;
        }

        return output;
    }

public:
    void init(Rfid *driverInstance) { instance = driverInstance; }

    void manageUid()
    {
        MFRC522 rfid = instance->getRfid();

        // Reset the loop if no new card present on the sensor/reader. This
        // saves the entire process when idle.
        if (!rfid.PICC_IsNewCardPresent())
            return;

        // Verify if the NUID has been readed
        if (!rfid.PICC_ReadCardSerial())
            return;

        // Record UID
        String uidHex = convertHexToString(rfid.uid.uidByte, rfid.uid.size);
        Serial.println(uidHex);

        // Halt PICC
        rfid.PICC_HaltA();

        // Stop encryption on PCD
        rfid.PCD_StopCrypto1();
    }
};