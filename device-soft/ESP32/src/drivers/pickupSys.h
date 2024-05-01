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
            analogWrite(anodePin, 255);
        }
        else
        {
            analogWrite(anodePin, 0);
        }
    }

    void initBuzz() { active = true; }
    void stopBuzz() { active = false; }
};

class IR_SensDriver
{
private:
    int signalPin;
    bool armed;

    bool detected;
    bool picked;

    unsigned long armTime;

public:
    IR_SensDriver(int pin) : signalPin(pin)
    {
        reset();
    }

    void reset()
    {
        armed = false;
        armTime = 0;
        detected = false;
        picked = false;
    }

    void arm()
    {
        if (!armed)
        {
            reset();
            armed = true;
            armTime = millis();

            Serial.println("Sensor armed!");
        }
    }

    void detect()
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

    bool getArmed()
    {
        return armed;
    }

    unsigned long getArmTime()
    {
        return armTime;
    }

    int check()
    {
        if (millis() - armTime > 10000)
        {
            reset();
            return -1;
        }
        else if (millis() - armTime > 5000)
        {
            if (!detected)
            {
                reset();
                return 3;
            }
            else if (detected && !picked)
            {
                return 2;
            }
            else
            {
                reset();
                return 1;
            }
        }
        else
        {
            return 0;
        }
    }
};