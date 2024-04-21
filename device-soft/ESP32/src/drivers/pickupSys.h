#define CSH_FREQ 277

class PiezoBuzzerDriver
{
private:
    int anodePin;
    bool active;

public:
    PiezoBuzzerDriver(int pin) : anodePin(pin), active(false) {}

    void buzz()
    {
        if (active)
        {
            tone(anodePin, CSH_FREQ);
        }
        else
        {
            tone(anodePin, 0);
        }
    }

    void initBuzz() { active = true; }
    void stopBuzz() { active = false; }
};

class IRSensDriver
{
private:
    int signalPin;
    bool armed;

    bool detected;
    bool picked;

public:
    IRSensDriver(int pin) : signalPin(pin)
    {
        reset();
    }

    void reset()
    {
        armed = false;
        detected = false;
        picked = false;
    }

    void arm()
    {
        if (!armed)
        {
            reset();
            armed = true;
        }
    }

    void detect()
    {
        if (armed)
        {
            if (!detected)
            {
                if (digitalRead(signalPin) == LOW)
                {
                    detected = true;
                }
            }
            else
            {
                if (digitalRead(signalPin) == HIGH)
                {
                    picked = true;
                }
            }
        }
    }

    bool check()
    {
        bool result = detected && picked;
        reset();
        return result;
    }
};