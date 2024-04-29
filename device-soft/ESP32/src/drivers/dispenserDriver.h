class DispenserDriver
{
private:
    Servo *pMotor;
    int initAngle = 70;
    int rotUnit = 36;

public:
    DispenserDriver(Servo *pm) : pMotor(pm)
    {
        pMotor->write(initAngle);
    }

    void dispence(int cRelNum)
    {
        pMotor->write(initAngle + cRelNum * rotUnit);
        delay(1000);
        pMotor->write(initAngle);
    }
};

class DispenceSequence
{
private:
    int sequence[32];
    int seqLen;
    DispenserDriver *md;

public:
    DispenceSequence(DispenserDriver *driver) : seqLen(0), md(driver) {}

    int dispence()
    {
        md->dispence(sequence[seqLen - 1]);

        seqLen--;
        for (int i = 0; i < seqLen; i++)
        {
            sequence[i] = sequence[i + 1];
        }

        return seqLen;
    }

    void pushToSeq(int cNum)
    {
        sequence[seqLen] = (cNum == 2) ? 1 : -1;
        seqLen++;
    }

    bool checkSeq()
    {
        return seqLen > 0;
    }

    void displaySequence()
    {
        for (int i = 0; i < seqLen; i++)
        {
            Serial.print(sequence[i]);
            Serial.print(", ");
        }

        Serial.println("\n");
    }
};