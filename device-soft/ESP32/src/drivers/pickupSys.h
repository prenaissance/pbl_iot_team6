#define CSH_FREQ 277

#define STOP -1
#define PICKUP_ARMED_IDLE 0
#define PICKUP_SUCCESS 1
#define PICKUP_FAILURE 2
#define MECHANISM_FAILURE_STOP 3
#define PICKUP_FAILURE_STOP 4

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
        if (millis() - armTime > 25000)
        {
            if (detected && !picked)
            {
                reset();
                return PICKUP_FAILURE_STOP;
            }
            else
            {
                reset();
                return MECHANISM_FAILURE_STOP;
            }
        }
        else if (millis() - armTime > 15000)
        {
            if (!detected)
            {
                reset();
                return MECHANISM_FAILURE_STOP;
            }
            else if (detected && !picked)
            {
                return PICKUP_FAILURE;
            }
            else
            {
                reset();
                return PICKUP_SUCCESS;
            }
        }
        else
        {
            if (detected && picked)
            {
                reset();
                return PICKUP_SUCCESS;
            }
            else
            {
                return PICKUP_ARMED_IDLE;
            }
        }
    }
};