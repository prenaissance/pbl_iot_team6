#include <SPI.h>
#include <MFRC522.h>

#define MAX_KEY_SIZE 6

class RfidDriver
{
private:
    MFRC522 rfid;
    MFRC522::MIFARE_Key key;

    String toStringHex(byte *buffer, byte bufferSize)
    {
        char charArray[bufferSize + 1];

        for (int i = 0; i < bufferSize; i++)
        {
            charArray[i] = (char)buffer[i];
        }

        charArray[bufferSize] = '\0';

        return String(charArray);
    }

public:
    RfidDriver() : rfid(0, 0)
    {
    }

    RfidDriver(int ssPin, int rstPin) : rfid(ssPin, rstPin)
    {
        SPI.begin();     // Init SPI bus
        rfid.PCD_Init(); // Init MFRC522

        for (byte i = 0; i < MAX_KEY_SIZE; ++i)
        {
            key.keyByte[i] = 0xFF;
        }
    }

    String getUid()
    {
        return toStringHex(rfid.uid.uidByte, rfid.uid.size);
    }

    MFRC522 getRfid()
    {
        return rfid;
    }
};

class RfidDriverManager
{
private:
    RfidDriver *instance;

public:
    void init(RfidDriver *driverInstance)
    {
        instance = driverInstance;
    }

    void manageUid()
    {
        // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
        if (!instance->getRfid().PICC_IsNewCardPresent())
            return;

        // Verify if the NUID has been readed
        if (!instance->getRfid().PICC_ReadCardSerial())
            return;

        // Record UID
        String uidHex = instance->getUid();
        Serial.println(uidHex);

        // Halt PICC
        instance->getRfid().PICC_HaltA();

        // Stop encryption on PCD
        instance->getRfid().PCD_StopCrypto1();
    }
};