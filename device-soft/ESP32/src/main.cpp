#include <Arduino.h>
#include <ArduinoJson.h>

#include "BluetoothSerial.h"

#include <MFRC522.h>
#include <SPI.h>

#include <ESP32Time.h>

#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

#include <iomanip>
#include <sstream>

#include "drivers/dispenserDriver.h"
#include "drivers/lcdDriver.h"
#include "drivers/ledDriver.h"
#include "drivers/pickupSys.h"
#include "drivers/rfidDriver.h"
#include "./deviceData.h"

#define BAUD_RATE 9600

#define SERVO_PIN 15

#define LED1_R 26
#define LED1_G 27

#define LED2_R 14
#define LED2_G 12

#define IR_PIN 13
#define PIEZO_PIN 32

#define I2C_ADDR 0x27
#define LCD_COLUMNS 16
#define LCD_LINES 2

#define SS_PIN 17
#define RST_PIN 4

#define BT_DEVICE_NAME "ESP32"

// Use this buffer for f-output

char outputBuffer[256];

// Device's id SRAM storage and conversion functions

uint8_t deviceIdBytes[LCD_COLUMNS];
std::string deviceIdBase64 = "";

std::string uint8_tToHexString(const uint8_t *, size_t);
std::string uuid4Format(const std::string &);

// Peripherals' drivers & managers

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);
static LcdDriver ld(&lcd);

#define UNRESOLVED_FAILURE -1
#define PICKUP_ARMED_IDLE 0
#define PICKUP_SUCCESS 1
#define PICKUP_FAILURE 2
#define MECHANISM_FAILURE 3

PiezoBuzzerDriver pd(PIEZO_PIN);
IR_SensDriver id(IR_PIN);

Servo motor;
static DispenserDriver dd(&motor);
static DispenceSequence ds(&dd);

static LedManager lm;

Rfid rfid;
static RfidDriver rm;

bool RFID_req;

// Device state SRAM storage and manip. functions

static DeviceData deviceData;

void updateSchedule();
void checkSchedule(String);

bool dataUpd;

// Main communication channel declaration

#define BT_TIME_UPD_REQ_CODE 0
#define BT_RFID_GET_REQ_CODE 1
#define BT_DUID_GET_REQ_CODE 2
#define BT_DATA_UPD_REQ_CODE 4

static BluetoothSerial SerialBT;
String receivedMsg;

void bluetoothReqHandler(String);

// RTC

ESP32Time rtc(0);
bool timeUpd;

std::string getCurrTimeFmt();
std::string getCurrDateFmt();

void setup()
{
    Wire.begin();
    Serial.begin(BAUD_RATE);

    Serial.println("\n\nSetup logs:\n");

    // LCD

    ld.accessQueue()->enqueue(LcdMsg("    Setup...    ", "", 1, 1));
    ld.update();

    lcd.init();
    lcd.backlight();

    Serial.println("I. LCD setup - success\n");

    // GPIO

    pinMode(SERVO_PIN, OUTPUT);

    pinMode(LED1_R, OUTPUT);
    pinMode(LED1_G, OUTPUT);

    pinMode(LED2_R, OUTPUT);
    pinMode(LED2_G, OUTPUT);

    pinMode(IR_PIN, INPUT);
    pinMode(PIEZO_PIN, OUTPUT);

    Serial.println("II. GPIO setup - success\n");

    // Device's UUID

    esp_fill_random(deviceIdBytes, sizeof(deviceIdBytes));
    deviceIdBase64 = uuid4Format(uint8_tToHexString(deviceIdBytes, 16));
    deviceIdBase64 = "11111111-1111-1111-1111-111111111111";

    sprintf(outputBuffer, "III. Device's UUID generated: %s\n", deviceIdBase64.c_str());
    Serial.println(outputBuffer);

    // BT

    SerialBT.begin(BT_DEVICE_NAME);

    // Motor

    motor.attach(SERVO_PIN);

    Serial.println("IV. Motor pinout set up successfully\n");

    // LED-s

    lm.init(LedDriver(LED1_R, LED1_G), 0);
    lm.update(0, 0);
    lm.init(LedDriver(LED2_R, LED2_G), 1);
    lm.update(0, 1);

    Serial.println("V. LED indication system initialized successfully\n");

    // RFID

    rfid.init(SS_PIN, RST_PIN);
    rm.init(&rfid);

    Serial.println("VI. RFID manager initialized successfully\n");

    timeUpd = false;
    dataUpd = false;
    RFID_req = false;

    ld.accessQueue()->enqueue(LcdMsg("     Setup      ", "   finsihed!    ", 1, 2000));
    ld.update();

    Serial.println("! Setup finished !\n");
}

void loop()
{
    while (SerialBT.available())
    {
        char incomingChar = SerialBT.read();

        if (incomingChar != '\n')
        {
            receivedMsg += String(incomingChar);
        }
        else
        {
            bluetoothReqHandler(receivedMsg);
            receivedMsg = "";
        }
    }

    if (!timeUpd || !dataUpd)
    {
        if (!timeUpd)
        {
            if (ld.accessQueue()->getQueueSize() == 0)
            {
                ld.accessQueue()->enqueue(LcdMsg("Set up sys time ", "via application ", 1, 1));
            }

            Serial.println("Time is not set up!");
        }
        else if (!dataUpd)
        {
            if (ld.accessQueue()->getQueueSize() == 0)
            {
                ld.accessQueue()->enqueue(LcdMsg("Set up sys time ", "via application ", 1, 1));
            }

            ld.accessQueue()->enqueue(LcdMsg("Upload sys data ", "via application ", 1, 1));
            Serial.println("Device data is not updated!");
        }
    }
    else
    {
        if (ds.checkSeq())
        {
            ds.displaySequence();
            ds.dispence();
        }
        else if (RFID_req)
        {
            if (rm.isNewCardRead())
            {
                rm.readUid();

                sprintf(outputBuffer, "Requested UUID: %s\n", rm.getCachedUid());
                Serial.println(outputBuffer);

                String RFID_data = rm.getCachedUid();

                String response = "\"{\"resCode\":" + String(BT_RFID_GET_REQ_CODE) + ", \"payload\":{\"rfid\":\"" + RFID_data + "\"}}\"";
                SerialBT.println(response);

                char line[LCD_COLUMNS + 1];
                sprintf(line, "    %s    ", RFID_data);
                ld.accessQueue()->enqueue(LcdMsg("   Tag read:    ", line, 1, 6000));

                Serial.println("Responded with RFID ID read\n");

                RFID_req = false;
            }
        }
        else if (!RFID_req)
        {
            if (rm.isNewCardRead())
            {
                rm.readUid();

                String RFID_data = rm.getCachedUid();

                char line[LCD_COLUMNS + 1];
                sprintf(line, "    %s    ", RFID_data);
                ld.accessQueue()->enqueue(LcdMsg("   Tag read:    ", line, 1, 2000));

                sprintf(outputBuffer, "Approached UUID: %s\n", rm.getCachedUid());
                Serial.println(outputBuffer);

                checkSchedule(rm.getCachedUid());
            }
        }
    }

    if (id.getArmed())
    {
        id.detect();

        switch (id.check())
        {
        case PICKUP_SUCCESS:
            pd.stopBuzz();

            Serial.println("Med-s were picked up!\n");
            ld.accessQueue()->enqueue(LcdMsg(" Healthier with ", "   IntelliDose  ", 1, 5000));

            break;

        case MECHANISM_FAILURE:
            Serial.println("Mechanism failure!\n");
            ld.accessQueue()->enqueue(LcdMsg(" Mechanism fai- ", " lure reported! ", 1, 1));

            break;

        case PICKUP_FAILURE:
            pd.initBuzz();

            Serial.println("Pickup failure!\n");
            ld.accessQueue()->enqueue(LcdMsg("    Take your   ", "   medicine!    ", 1, 1));

            break;

        case UNRESOLVED_FAILURE:
            pd.stopBuzz();

            Serial.println("Unresolved failure!\n");
            ld.accessQueue()->enqueue(LcdMsg("Unresolved fai- ", " lure reported! ", 1, 1));

            break;

        default:
            break;
        }
    }

    if (rm.getCachedUid() != "")
    {
        rm.clearCachedUid();
    }

    if (ld.accessQueue()->getQueueSize() == 0)
    {
        std::string fmt1 = getCurrTimeFmt();
        std::string fmt2 = getCurrDateFmt();

        char line1[LCD_COLUMNS + 1];
        char line2[LCD_COLUMNS + 1];

        sprintf(line1, fmt1.c_str(), rtc.getHour(true), rtc.getMinute());
        sprintf(line2, fmt2.c_str(), rtc.getDay(), rtc.getMonth() + 1, rtc.getYear());

        ld.accessQueue()->enqueue(LcdMsg(line1, line2, 1, 1));
    }

    ld.update();
    pd.buzz();

    delay(500);
}

std::string getCurrTimeFmt()
{
    std::string fmt;

    bool f1 = rtc.getHour(true) < 10;
    bool f2 = rtc.getMinute() < 10;

    if (f1 && f2)
    {
        fmt = "     0%d:0%d      ";
    }
    else if (f1)
    {
        fmt = "     0%d:%d      ";
    }
    else if (f2)
    {
        fmt = "     %d:0%d      ";
    }
    else
    {
        fmt = "     %d:%d      ";
    }

    return fmt;
}

std::string getCurrDateFmt()
{
    std::string fmt;

    bool f1 = rtc.getDay() < 10;
    bool f2 = rtc.getMonth() + 1 < 10;

    if (f1 && f2)
    {
        fmt = "   0%d/0%d/%d   ";
    }
    else if (f1)
    {
        fmt = "   0%d/%d/%d   ";
    }
    else if (f2)
    {
        fmt = "   %d/0%d/%d   ";
    }
    else
    {
        fmt = "   %d/%d/%d   ";
    }

    return fmt;
}

void bluetoothReqHandler(String reqString)
{
    char serializedData[4096];

    reqString.replace(" ", "");
    reqString.replace("\n", "");
    reqString.trim();
    reqString.remove(0, 1);
    reqString.toCharArray(serializedData, 3072);

    DynamicJsonDocument reqJSON(4096);
    DeserializationError err = deserializeJson(reqJSON, serializedData);

    if (err)
    {
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(err.f_str());
        Serial.println("\n");
    }

    int reqCode = reqJSON["reqCode"];
    JsonObject payload = reqJSON["payload"];

    switch (reqCode)
    {
    case BT_TIME_UPD_REQ_CODE:
    {
        int gmtOffset = payload["gmtOffset"];

        JsonObject date = payload["date"];
        int year = date["year"];
        int month = date["month"];
        int day = date["day"];

        JsonObject time = payload["time"];
        int hour = time["hour"];
        int minute = time["minute"];
        int second = time["second"];

        rtc.setTime(second, minute, hour, day, month, year);
        rtc.offset = gmtOffset;

        timeUpd = true;

        ld.accessQueue()->enqueue(LcdMsg("    Time upd    ", " successfully!  ", 1, 2000));

        Serial.println("Time successfully updated!");
    }
    break;
    case BT_DATA_UPD_REQ_CODE:
    {
        deviceData = DeviceData();

        // Record pill slots

        JsonArray pillSlots = payload["pillSlots"];
        for (JsonObject pillSlot : pillSlots)
        {
            int slotNumber = pillSlot["slotNumber"];
            const char *pillName = pillSlot["pillName"];
            int pillCount = pillSlot["pillCount"];

            deviceData.addPillSlot(PillSlot(pillName, pillCount), slotNumber);
            lm.update(pillCount, slotNumber - 1);
        }

        // Record profiles and their schedules

        JsonArray profiles = payload["profiles"];
        for (JsonObject profile : profiles)
        {
            const char *username = profile["username"];
            const char *RFID_UID = profile["rfid"];

            deviceData.addProfile(Profile(username, RFID_UID));

            Profile *pProf = deviceData.getProfile(String(RFID_UID));

            JsonArray pillSchedules = profile["pillSchedules"];
            for (JsonObject pillSchedule : pillSchedules)
            {
                int hour = pillSchedule["time"]["hour"];
                int minute = pillSchedule["time"]["minutes"];
                int slotNumber = pillSchedule["slotNumber"];
                int quantity = pillSchedule["quantity"];

                pProf->addItem(ScheduleItem(hour, minute, slotNumber, quantity));
            }
        }

        dataUpd = true;
        deviceData.status();

        ld.accessQueue()->enqueue(LcdMsg("  Device data   ", "   uploaded!    ", 1, 2000));

        Serial.println("Device data successfully updated\n");
    }
    break;

    case BT_DUID_GET_REQ_CODE:
    {
        String response = "\"{\"resCode\":" + String(BT_DUID_GET_REQ_CODE) + ", \"payload\":{\"deviceIdBase64\":\"" + String(deviceIdBase64.c_str()) + "\"}}\"";
        SerialBT.println(response);

        ld.accessQueue()->enqueue(LcdMsg(" Responded with ", " device's UUID  ", 1, 2000));

        Serial.println("Responded with device's UUID\n");
    }
    break;

    case BT_RFID_GET_REQ_CODE:
    {
        RFID_req = true;

        ld.accessQueue()->enqueue(LcdMsg("  RFID reading  ", "   requested    ", 1, 2000));
        ld.accessQueue()->enqueue(LcdMsg("    Approach    ", "    your tag    ", 1, 4000));

        Serial.println("RFID reading requset detected\n");
    }
    break;

    default:
        Serial.println("Unrecognized Bluetooth request!\n");

        break;
    }
}

void checkSchedule(String userRFID)
{
    char line1[LCD_COLUMNS + 1];
    char line2[LCD_COLUMNS + 1];

    bool found = false;

    Profile *pProf = deviceData.getProfile(userRFID);

    Serial.println(pProf->getUN());

    if (pProf != nullptr)
    {
        std::string fmt = getCurrTimeFmt();
        sprintf(line1, fmt.c_str(), rtc.getHour(true), rtc.getMinute());
        sprintf(line2, "      %s      ", pProf->getUN());

        ld.accessQueue()->enqueue(LcdMsg(line1, line2, 1, 5000));

        for (int i = 0; i < pProf->getSchedLen(); i++)
        {
            ScheduleItem *pItem = pProf->getItem(i);

            if (pItem->checkTime(rtc.getHour(true), rtc.getMinute()))
            {
                if (pItem->getFulfileld())
                {
                    sprintf(outputBuffer, "%d:%d schedule item for %s  was already fulfilled today!\n", pItem->getTimeH(), pItem->getTimeM(), pProf->getUN());
                    Serial.println(outputBuffer);

                    continue;
                }
                else
                {
                    found = true;

                    PillSlot *pSlot = deviceData.getPillSlot(pItem->getSlotNum());

                    if (pSlot == nullptr)
                    {
                        Serial.println("Slot not found!");
                        continue;
                    }
                    else
                    {
                        for (int j = 0; j < pItem->getQuantity(); j++)
                        {
                            if (!pSlot->checkPillCnt())
                            {
                                sprintf(outputBuffer, "Insufficient supply in slot: %d\n", pItem->getSlotNum());
                                Serial.println(outputBuffer);

                                break;
                            }
                            else
                            {
                                ds.pushToSeq(pItem->getSlotNum());
                                id.arm();

                                pSlot->decPillCnt();
                                pItem->check();

                                sprintf(outputBuffer, "%d:%d | slot: %d\n", pItem->getTimeH(), pItem->getTimeM(), pItem->getSlotNum());
                                Serial.println(outputBuffer);
                            }
                        }

                        lm.update(pSlot->getPillCnt(), pItem->getSlotNum() - 1);
                    }
                }
            }
        }
    }

    if (!found)
    {
        Serial.println("No schedule items found!\n");
    }
}

std::string uint8_tToHexString(const uint8_t *data, size_t len)
{
    std::stringstream ss;
    ss << std::hex;

    for (size_t i = 0; i < len; ++i)
    {
        ss << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]);
    }

    return ss.str();
}

std::string uuid4Format(const std::string &uuid_str)
{
    std::string formatted_uuid =
        uuid_str.substr(0, 8) + "-" +
        uuid_str.substr(8, 4) + "-" +
        uuid_str.substr(12, 4) + "-" +
        uuid_str.substr(16, 4) + "-" +
        uuid_str.substr(20);

    return formatted_uuid;
}