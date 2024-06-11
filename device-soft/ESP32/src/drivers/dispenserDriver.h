class DispenserDriver
{
private:
    Servo *pMotor;
    int initAngle = 73;
    int angle[2];

public:
    DispenserDriver(Servo *pm) : pMotor(pm)
    {
        pMotor->write(initAngle);
        angle[0] = -52;
        angle[1] = 40;
    }

    void dispence(int cNum)
    {
        pMotor->write(initAngle + angle[cNum]);
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
        md->dispence(sequence[0]);

        seqLen--;
        for (int i = 0; i < seqLen; i++)
        {
            sequence[i] = sequence[i + 1];
        }

        return seqLen;
    }

    void pushToSeq(int cNum)
    {
        sequence[seqLen] = cNum - 1;
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

        Serial.print("\n");
    }
};