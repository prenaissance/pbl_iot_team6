#include <Arduino.h>

class ScheduleItem
{
private:
    int time[2];
    int slotNum;
    int quantity;
    bool fulfilled;

public:
    ScheduleItem() {}
    ScheduleItem(int h, int m, int sn, int q) : time({h, m}), slotNum(sn), fulfilled(false) {}

    int getTimeH()
    {
        return time[0];
    }

    int getTimeM()
    {
        return time[1];
    }

    int getSlotNum()
    {
        return slotNum;
    }

    int getQuantity()
    {
        return quantity;
    }

    bool getFulfileld()
    {
        return fulfilled;
    }

    void check()
    {
        fulfilled = true;
    }

    void uncheck()
    {
        fulfilled = false;
    }

    bool checkTime(int currTime[2])
    {
        return abs((time[0] * 60 + time[1]) - (currTime[0] * 60 + currTime[1])) < 30;
    };
};

class PillSlot
{
private:
    char pillName[20];
    int pillCnt;

public:
    PillSlot() {}
    PillSlot(const char *pn, int pc) : pillCnt(pc)
    {
        strcpy(pillName, pn);
    }

    void decPillCnt()
    {
        pillCnt--;
    }

    int getPillCnt()
    {
        return pillCnt;
    }

    bool checkPillCnt()
    {
        return pillCnt > 0;
    }

    char *getPillName()
    {
        return pillName;
    }
};

class Profile
{
private:
    char username[20];
    char RFID_UID[10];

    ScheduleItem schedule[20];
    int scheduleSize;

public:
    Profile() {}
    Profile(const char *un, const char *ruid) : scheduleSize(0)
    {
        strcpy(username, un);
        strcpy(RFID_UID, ruid);
    }

    char *getUN()
    {
        return username;
    }

    char *getRUID()
    {
        return RFID_UID;
    }

    int getSchedLen()
    {
        return scheduleSize;
    }

    ScheduleItem *getItem(int idx)
    {
        if (idx >= scheduleSize)
        {
            return nullptr;
        }
        else
        {
            return &schedule[idx];
        }
    }

    void addItem(ScheduleItem si)
    {
        if (scheduleSize < 20)
        {
            schedule[scheduleSize] = si;
            scheduleSize++;
        }
        else
        {
            Serial.println("Schedule overflow!");
        }
    }
};

class DeviceData
{
private:
    Profile profiles[10];
    int profilesSize;

    PillSlot pillSlots[2];

public:
    DeviceData() : profilesSize(0) {}

    void addProfile(Profile prof)
    {
        if (profilesSize < 10)
        {
            profiles[profilesSize] = prof;
            profilesSize++;
        }
        else
        {
            Serial.println("Profiles overflow!");
        }
    }

    Profile *getProfile(String ruid)
    {
        Profile *profPtr = nullptr;

        for (int i = 0; i < profilesSize; i++)
        {
            if (ruid.equals(profiles[i].getRUID()))
            {
                profPtr = &profiles[i];
                break;
            }
        }

        return profPtr;
    }

    void addPillSlot(PillSlot ps, int sNum)
    {
        if (sNum > 2)
        {
            Serial.println("Invalid slotNumber!");
        }
        else
        {
            pillSlots[sNum - 1] = ps;
        }
    }

    PillSlot *getPillSlot(int sNum)
    {
        PillSlot *slotPtr = nullptr;

        if (sNum > 0 && sNum < 3)
        {
            slotPtr = &pillSlots[sNum - 1];
        }

        return slotPtr;
    }
};