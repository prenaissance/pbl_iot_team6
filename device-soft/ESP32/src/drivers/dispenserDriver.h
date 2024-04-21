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
        delay(500);
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
        md->dispence(sequence[seqLen]);

        seqLen--;
        for (int i = 0; i < seqLen; i++)
        {
            sequence[i] = sequence[i + 1];
        }

        return seqLen;
    }

    void pushToSeq(int cNum)
    {
        sequence[seqLen] = (cNum == 2) ? -1 : cNum;
        seqLen++;
    }
};