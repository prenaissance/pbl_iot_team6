#include <Arduino.h>
#include <ArduinoJson.h>

#include "BluetoothSerial.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include <MFRC522.h>
#include <SPI.h>

#include <ESP32Time.h>

#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <Preferences.h>

#include <iomanip>
#include <sstream>

#include "drivers/dispenserDriver.h"
#include "drivers/lcdDriver.h"
#include "drivers/ledDriver.h"
#include "drivers/pickupSys.h"
#include "drivers/rfidDriver.h"
#include "./deviceData.h"
#include "./cert.h"

#define BAUD_RATE 9600

#define DD_TRANSM_SIZE 2048

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

#define SDA_PIN 21
#define SCL_PIN 22

#define SS_PIN 17
#define RST_PIN 4

#define BT_DEVICE "ESP32"

#define DEVICE_PARAM_NAMESPACE "device-param"
#define DEVICE_ID_SELECTOR "device-id"

// Use this buffer for f-output

char outputBuffer[256];

// Handler used for managing variables in nvm
Preferences preferences;

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

void saveData(JsonObject);
void checkSchedule(String);

// BT

#define BT_RFID_GET_REQ 1
#define BT_DUID_GET_REQ 2
#define BT_WIFI_SET_REQ 3
#define BT_DATA_UPD_REQ 4

static BluetoothSerial SerialBT;
String receivedMsg;

void bluetoothReqHandler(String);

// RTC

ESP32Time rtc(0);

// WiFi

bool switchToWiFi();
void switchToBT();

WiFiClientSecure wsClient;

#define WIFI_CONNECT_TIMEOUT 4000

char ssid[50];
char pass[50];
unsigned long conTimer;
unsigned long lastConAtt;

// TIME

#define TIME_SRC "http://worldtimeapi.org/api/ip/%s"

bool httpTimeUpd();

// HTTP

HTTPClient client;

#define PUB_IP_LOOKUP "http://api.ipify.org"

char ipStr[16];
void pubIP_lookup();

#define DEVICE_COMMON_ROUTE_PART "https://dispenser-backend.onrender.com/api/devices/%s"
#define DATA_ENDPOINT_PART "/config"
#define EVENT_ENDPOINT_PART "/event"

#define CREATE_EVENT_PARAMS_FMT "?eventType=%s&eventData=%s"
#define CREATE_EVENT_PARAMS_FMT_TRAILER "&profileId=%d"

#define EVENT_FMT "{\"eventType\":\"%s\",\"eventData\":\"%s\"}"
#define EVENT_ASSIGNMENT_TRAILER "\"profileId\":%d}"

void httpsDataUpd();
void httpsCreateEventViaPOST(String, String, unsigned int);
void httpsCreateEventViaGET(String, String, unsigned int);

StaticJsonDocument<DD_TRANSM_SIZE> json;

bool startupLog;

void setup()
{
    // INTERFACES

    Serial.begin(BAUD_RATE);
    Wire.begin();

    Serial.println("\n\nSetup logs:");

    // LCD

    ld.accessQueue()->enqueue(LcdMsg("    Setup...    ", "", 1, 1));
    ld.update();

    lcd.init();
    lcd.backlight();

    Serial.println("1. LCD setup - success");

    // GPIO

    pinMode(SERVO_PIN, OUTPUT);

    pinMode(LED1_R, OUTPUT);
    pinMode(LED1_G, OUTPUT);

    pinMode(LED2_R, OUTPUT);
    pinMode(LED2_G, OUTPUT);

    pinMode(IR_PIN, INPUT);
    pinMode(PIEZO_PIN, OUTPUT);

    Serial.println("2. GPIO setup - success");

    // Device's UUID

    preferences.begin(DEVICE_PARAM_NAMESPACE, false);
    String savedDeviceIdBase64 = preferences.getString(DEVICE_ID_SELECTOR, "");

    if (savedDeviceIdBase64 != "")
    {
        deviceIdBase64 = savedDeviceIdBase64.c_str();

        sprintf(outputBuffer, "3. Device UUID extracted: %s", deviceIdBase64.c_str());
        Serial.println(outputBuffer);
    }
    else
    {
        esp_fill_random(deviceIdBytes, sizeof(deviceIdBytes));
        deviceIdBase64 = uuid4Format(uint8_tToHexString(deviceIdBytes, 16));
        preferences.putString(DEVICE_ID_SELECTOR, deviceIdBase64.c_str());

        sprintf(outputBuffer, "3. Device UUID generated: %s", deviceIdBase64.c_str());
        Serial.println(outputBuffer);
    }

    preferences.end();

    deviceIdBase64 = "11111111-1111-1111-1111-111111111111";

    // Motor

    motor.attach(SERVO_PIN);

    Serial.println("4. Motor pinout set up successfully");

    // LED-s

    lm.init(LedDriver(LED1_R, LED1_G), 0);
    lm.update(0, 0);
    lm.init(LedDriver(LED2_R, LED2_G), 1);
    lm.update(0, 1);

    Serial.println("5. LED indication system initialized successfully");

    // RFID

    rfid.init(SS_PIN, RST_PIN);
    rm.init(&rfid);

    Serial.println("6. RFID manager initialized successfully");

    // BT

    SerialBT.begin(BT_DEVICE);

    Serial.println("7. BT operational");

    // WIFI

    wsClient.setCACert(cert_chain);

    Serial.println("8. WiFi Secure certificate set up");

    // FINISH

    startupLog = false;
    RFID_req = false;

    conTimer = 0;

    ld.accessQueue()->enqueue(LcdMsg("     Setup      ", "   finsihed!    ", 1, 1));
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

    if (strlen(ssid) > 0 && strlen(pass) > 0)
    {
        bool timeCheck = rtc.getYear() > 1970;
        bool dataCheck = deviceData.getProfilesSize() > 0;
        bool ipCheck = strlen(ipStr) > 0;

        if (!timeCheck || !dataCheck || !ipCheck)
        {
            if (!ipCheck)
            {
                ld.accessQueue()->enqueue(LcdMsg("    Public IP   ", "     lookup     ", 1, 1));
                ld.update();
                Serial.println("Public ip is not known!");
                pubIP_lookup();
            }
            else if (!timeCheck)
            {
                ld.accessQueue()->enqueue(LcdMsg("    Datetime    ", "     lookup    ", 1, 1));
                ld.update();
                Serial.println("Time is not set up!");

                if (httpTimeUpd())
                {
                    std::string fmt1 = getTimeFmt(rtc.getHour(true) < 10, rtc.getMinute() < 10);
                    std::string fmt2 = getDateFmt(rtc.getDay() < 10, rtc.getMonth() + 1 < 10);

                    char line1[LCD_COLUMNS + 1];
                    char line2[LCD_COLUMNS + 1];

                    sprintf(line1, fmt1.c_str(), rtc.getHour(true), rtc.getMinute());
                    sprintf(line2, fmt2.c_str(), rtc.getDay(), rtc.getMonth() + 1, rtc.getYear());

                    Serial.println(line1);
                    Serial.println(line2);
                    Serial.print("\n");
                }
            }
            else if (!dataCheck)
            {
                ld.accessQueue()->enqueue(LcdMsg("   Config-data   ", "    NOT FOUND   ", 1, 1));
                Serial.println("Device data is not updated!");
            }
        }
        else
        {
            if (!startupLog)
            {
                sprintf(outputBuffer, "{\\\"msg\\\":\\\"IntelliDose~%s has begun operation\\\"}", deviceIdBase64.c_str());

                httpsCreateEventViaPOST("LOG", String(outputBuffer), 0);
                startupLog = true;

                ld.accessQueue()->enqueue(LcdMsg(" Device - fully ", "   operational!  ", 1, 1));
            }

            if (ds.checkSeq())
            {
                // ds.displaySequence();
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

                    String response = "\"{\"resCode\":" + String(BT_RFID_GET_REQ) + ", \"payload\":{\"rfid\":\"" + RFID_data + "\"}}\"";
                    SerialBT.println(response);

                    char line[LCD_COLUMNS + 1];
                    sprintf(line, "    %s    ", RFID_data);
                    ld.accessQueue()->enqueue(LcdMsg("   Tag read:    ", line, 1, 1));

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
                    ld.accessQueue()->enqueue(LcdMsg("   Tag read:    ", line, 1, 1));

                    sprintf(outputBuffer, "Approached UUID: %s - ", rm.getCachedUid());
                    Serial.print(outputBuffer);

                    checkSchedule(rm.getCachedUid());
                }
            }
        }
    }
    else
    {
        Serial.println("Provide WiFi details!");
        ld.accessQueue()->enqueue(LcdMsg("  Awaiting WiFi ", "     details    ", 1, 1));
    }

    if (id.getArmed())
    {
        id.detect();

        switch (id.check())
        {
        case PICKUP_SUCCESS:
            pd.stopBuzz();

            Serial.println("Med-s were picked up!\n");
            ld.accessQueue()->enqueue(LcdMsg(" Healthier with ", "   IntelliDose  ", 1, 1));

            break;

        case MECHANISM_FAILURE:
            Serial.println("Mechanism failure!\n");
            ld.accessQueue()->enqueue(LcdMsg(" Mechanism fai- ", " lure reported! ", 1, 1));

            httpsCreateEventViaPOST("LOG", "{\\\"msg\\\":\\\"During the dispensing procedure, a mechanism failure occurred in the device!\\\"}", 0);

            break;

        case PICKUP_FAILURE:
            pd.initBuzz();

            Serial.println("Pickup failure!\n");
            ld.accessQueue()->enqueue(LcdMsg("    Take your   ", "   medicine!    ", 1, 1));

            httpsCreateEventViaPOST("LOG", "{\\\"msg\\\":\\\"The dispensed medicine was not picked up!\\\"}", 0);

            break;

        case UNRESOLVED_FAILURE:
            pd.stopBuzz();

            Serial.println("Unresolved failure!\n");
            ld.accessQueue()->enqueue(LcdMsg("Unresolved fai- ", " lure reported! ", 1, 1));

            httpsCreateEventViaPOST("LOG", "{\\\"msg\\\":\\\"During the dispensing procedure, an unresolved failure occurred in the device!\\\"}", 0);

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
        std::string fmt1 = getTimeFmt(rtc.getHour(true) < 10, rtc.getMinute() < 10);
        std::string fmt2 = getDateFmt(rtc.getDay() < 10, rtc.getMonth() + 1 < 10);

        char line1[LCD_COLUMNS + 1];
        char line2[LCD_COLUMNS + 1];

        sprintf(line1, fmt1.c_str(), rtc.getHour(true), rtc.getMinute());
        sprintf(line2, fmt2.c_str(), rtc.getDay(), rtc.getMonth() + 1, rtc.getYear());

        ld.accessQueue()->enqueue(LcdMsg(line1, line2, 1, 1));
    }

    ld.update();
    pd.buzz();

    delay(1000);
}

void bluetoothReqHandler(String reqString)
{
    char serializedData[DD_TRANSM_SIZE];

    reqString.replace("\n", "");
    reqString.trim();
    reqString.remove(0, 1);
    reqString.toCharArray(serializedData, DD_TRANSM_SIZE);

    json.clear();
    DeserializationError err = deserializeJson(json, serializedData);

    if (err)
    {
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(err.f_str());
        Serial.println("\n");

        return;
    }

    int reqCode = json["reqCode"];
    JsonObject payload = json["payload"];

    switch (reqCode)
    {
    case BT_DUID_GET_REQ:
    {
        String response = "\"{\"resCode\":" + String(BT_DUID_GET_REQ) + ", \"payload\":{\"deviceIdBase64\":\"" + String(deviceIdBase64.c_str()) + "\"}}\"";
        SerialBT.println(response);

        ld.accessQueue()->enqueue(LcdMsg(" Responded with ", " device's UUID  ", 1, 1));

        Serial.println("Responded with device's UUID\n");
    }
    break;

    case BT_RFID_GET_REQ:
    {
        RFID_req = true;

        ld.accessQueue()->enqueue(LcdMsg("  RFID reading  ", "   requested    ", 1, 1));
        ld.accessQueue()->enqueue(LcdMsg("    Approach    ", "    your tag    ", 1, 1));

        Serial.println("RFID reading requset detected\n");
    }
    break;

    case BT_WIFI_SET_REQ:
    {
        strcpy(ssid, payload["ssid"]);
        strcpy(pass, payload["pass"]);

        Serial.println("\nWiFi details received\n");
    }
    break;

    case BT_DATA_UPD_REQ:
    {
        Serial.println("\nData upd requested");
        httpsDataUpd();
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
        std::string fmt = getTimeFmt(rtc.getHour(true) < 10, rtc.getMinute() < 10);
        sprintf(line1, fmt.c_str(), rtc.getHour(true), rtc.getMinute());

        String nameFmt;
        for (int i = 0; i < (LCD_COLUMNS - strlen(pProf->getUN())) / 2; i++)
        {
            nameFmt += ' ';
        }
        nameFmt += "%s";

        sprintf(line2, nameFmt.c_str(), pProf->getUN());

        ld.accessQueue()->enqueue(LcdMsg(line1, line2, 1, 1));
        ld.update();

        for (int i = 0; i < pProf->getSchedLen(); i++)
        {
            ScheduleItem *pItem = pProf->getItem(i);

            if (pItem->checkTime(rtc.getHour(true), rtc.getMinute(), 120))
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
                                sprintf(outputBuffer, "{\\\"msg\\\":\\\"The slot #%d has run out of supplies!\\\"}", pItem->getSlotNum());
                                httpsCreateEventViaPOST("LOG", outputBuffer, 0);

                                break;
                            }
                            else
                            {
                                ds.pushToSeq(pItem->getSlotNum());
                                id.arm();

                                pSlot->decPillCnt();
                                pItem->check();

                                sprintf(outputBuffer, "{\\\"msg\\\":\\\"x1 %s provided to %s\\\"}", pSlot->getPillName(), pProf->getUN());
                                httpsCreateEventViaPOST("LOG", outputBuffer, pProf->getProfileId());

                                // if (!pSlot->checkPillCnt())
                                // {
                                //     sprintf(outputBuffer, "{\\\"msg\\\":\\\"The slot #%d has run out of supplies!\\\"}", pItem->getSlotNum());
                                //     httpsCreateEventViaPOST("LOG", outputBuffer, 0);
                                // }
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
        Serial.println(" Unknown!");
        Serial.println("No schedule items found!\n");
    }
}

bool httpTimeUpd()
{
    if (!switchToWiFi())
    {
        Serial.println("IP lookup failed due to WiFi connection failure!\n");
        ld.accessQueue()->enqueue(LcdMsg("  FAILED due to ", "WiFi con. issues", 1, 1));
        return false;
    }

    Serial.println(" Connected successfully!");

    char url[100];
    sprintf(url, TIME_SRC, ipStr);

    client.begin(url);
    int httpResCode = client.GET();

    if (httpResCode >= 200 && httpResCode < 300)
    {
        json.clear();
        DeserializationError err = deserializeJson(json, client.getString());

        if (err)
        {
            ld.accessQueue()->enqueue(LcdMsg("    Operation   ", "     FAILED!    ", 1, 1));
            sprintf(outputBuffer, "Time update: deserializeJson() failed - %s!", err.c_str());
            Serial.println(outputBuffer);
            return false;
        }

        String datetime = json["datetime"];

        struct DateTime
        {
            int year;
            int month;
            int day;
            int hour;
            int minute;
            int second;
            int ms;
            int offsetHrs;
        };

        DateTime dt;
        int pc = sscanf(
            datetime.c_str(),
            "%d-%d-%dT%d:%d:%d.%d+%d:00",
            &dt.year, &dt.month, &dt.day, &dt.hour, &dt.minute, &dt.second, &dt.ms, &dt.offsetHrs);

        if (pc == 8)
        {
            rtc.setTime(dt.second, dt.minute, dt.hour, dt.day, dt.month, dt.year);
            rtc.offset = 0;

            ld.accessQueue()->enqueue(LcdMsg("  Time updated  ", "  successfully! ", 1, 1));
            Serial.println("Time update: Time successfully updated");
        }
        else
        {
            ld.accessQueue()->enqueue(LcdMsg("    Operation   ", "     FAILED!    ", 1, 1));
            Serial.println("Time update: Error parsing datetime string!");
            return false;
        }
    }
    else
    {
        ld.accessQueue()->enqueue(LcdMsg("    Operation   ", "     FAILED!    ", 1, 1));
        sprintf(outputBuffer, "Time update: Error on HTTP request - %s!", client.errorToString(httpResCode));
        Serial.println(outputBuffer);
    }

    client.end();
    switchToBT();

    return true;
}

void httpsDataUpd()
{
    if (!switchToWiFi())
    {
        Serial.println("Data update failed due to WiFi connection failure!\n");
        ld.accessQueue()->enqueue(LcdMsg("  FAILED due to ", "WiFi con. issues", 1, 1));
        return;
    }

    Serial.println(" Connected successfully!");

    char url[100];
    sprintf(url, DEVICE_COMMON_ROUTE_PART, deviceIdBase64.c_str());
    strcat(url, DATA_ENDPOINT_PART);

    client.begin(wsClient, url);
    int httpResCode = client.GET();

    if (httpResCode >= 200 && httpResCode < 300)
    {
        json.clear();
        DeserializationError err = deserializeJson(json, client.getString());

        if (err)
        {
            ld.accessQueue()->enqueue(LcdMsg("    Operation   ", "     FAILED!    ", 1, 1));
            sprintf(outputBuffer, "deserializeJson() failed: %s", err.c_str());
            Serial.println(outputBuffer);

            return;
        }

        saveData(json.as<JsonObject>());

        deviceData.status();

        ld.accessQueue()->enqueue(LcdMsg("Config-data upd.", "  successfully! ", 1, 1));
        Serial.println("Data update: Device data successfully updated");
    }
    else
    {
        ld.accessQueue()->enqueue(LcdMsg("    Operation   ", "     FAILED!    ", 1, 1));
        sprintf(outputBuffer, "Data update: Error on HTTPS request - %s!", client.errorToString(httpResCode));
        Serial.println(outputBuffer);
    }

    client.end();
    switchToBT();
}

void httpsCreateEventViaPOST(String eventType, String eventData, unsigned int profileId)
{
    if (!switchToWiFi())
    {
        Serial.println("Event posting failed due to WiFi connection failure!\n");
        return;
    }

    Serial.println(" Connected successfully!");

    char url[100];
    sprintf(url, DEVICE_COMMON_ROUTE_PART, deviceIdBase64.c_str());
    strcat(url, EVENT_ENDPOINT_PART);

    char reqBody[400];
    sprintf(reqBody, EVENT_FMT, eventType.c_str(), eventData.c_str());

    if (profileId > 0)
    {
        char trailer[20];
        sprintf(trailer, EVENT_ASSIGNMENT_TRAILER, profileId);

        reqBody[strlen(reqBody) - 1] = ',';
        strcat(reqBody, trailer);
    }

    Serial.print("POST event: ");
    Serial.println(String(reqBody));

    client.begin(wsClient, url);
    client.addHeader("Content-Type", "application/json");

    int httpResCode = client.POST(String(reqBody));

    if (httpResCode >= 200 && httpResCode < 300)
    {
        Serial.println("Event posting: Success");
    }
    else
    {
        sprintf(outputBuffer, "Event posting: Error on HTTPS request - %d!", httpResCode);
        Serial.println(outputBuffer);
    }

    client.end();
    switchToBT();
}

void httpsCreateEventViaGET(String eventType, String eventData, unsigned int profileId)
{
    if (!switchToWiFi())
    {
        Serial.println("Event posting via GET failed due to WiFi connection failure!\n");
        return;
    }

    Serial.println(" Connected successfully!");

    char url[500];
    sprintf(url, DEVICE_COMMON_ROUTE_PART, deviceIdBase64.c_str());
    strcat(url, EVENT_ENDPOINT_PART);

    char params[400];

    sprintf(params, CREATE_EVENT_PARAMS_FMT, eventType.c_str(), eventData.c_str());

    String translatedParams = String(params);

    translatedParams.replace("{", "%7B");
    translatedParams.replace("}", "%7D");
    translatedParams.replace("\"", "%22");
    translatedParams.replace(":", "%3A");
    translatedParams.replace(" ", "%20");

    strcpy(params, translatedParams.c_str());

    if (profileId > 0)
    {
        char trailer[20];
        sprintf(trailer, CREATE_EVENT_PARAMS_FMT_TRAILER, profileId);

        strcat(params, trailer);
    }

    strcat(url, params);

    client.begin(url);
    int httpResCode = client.GET();

    if (httpResCode >= 200 && httpResCode < 300)
    {
        Serial.println("Event posting via GET: Success");
    }
    else
    {
        sprintf(outputBuffer, "Event posting via GET: Error on HTTPS request - %d!", httpResCode);
        Serial.println(outputBuffer);
    }

    client.end();
    switchToBT();
}

void saveData(JsonObject payload)
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
        unsigned int profileId = profile["profileId"];
        const char *username = profile["username"];
        const char *RFID_UID = profile["rfid"];

        deviceData.addProfile(Profile(profileId, username, RFID_UID));

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
}

void pubIP_lookup()
{
    if (!switchToWiFi())
    {
        Serial.println("IP lookup failed due to WiFi connection failure!\n");
        ld.accessQueue()->enqueue(LcdMsg("  FAILED due to ", "WiFi con. issues", 1, 1));
        return;
    }

    Serial.println(" Connected successfully!");

    client.begin(PUB_IP_LOOKUP);
    int httpResCode = client.GET();

    if (httpResCode > 0)
    {
        strcpy(ipStr, client.getString().c_str());

        ld.accessQueue()->enqueue(LcdMsg("  IP retrieved  ", "  successfully! ", 1, 1));
        sprintf(outputBuffer, "Public IP address lookup: success - %s", ipStr);
        Serial.println(outputBuffer);
    }
    else
    {
        ld.accessQueue()->enqueue(LcdMsg("    Operation   ", "     FAILED!    ", 1, 1));
        sprintf(outputBuffer, "Public IP lookup: Error on HTTP request - %s!", client.errorToString(httpResCode));
        Serial.println(outputBuffer);
    }

    client.end();
    switchToBT();
}

bool switchToWiFi()
{
    sprintf(outputBuffer, "Disabling BT... connecting to %s: ..", ssid);
    Serial.print(outputBuffer);

    SerialBT.end();

    lastConAtt = millis();
    conTimer = 0;

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(250);

        Serial.print(".");

        conTimer += millis() - lastConAtt;
        lastConAtt = millis();

        if (conTimer > WIFI_CONNECT_TIMEOUT)
        {
            memset(ssid, '\0', sizeof(ssid));
            memset(pass, '\0', sizeof(pass));

            Serial.println("Attempt timed out - WiFi details erased! Restarting BT");

            WiFi.disconnect(true);
            WiFi.mode(WIFI_OFF);

            SerialBT.begin(BT_DEVICE);

            return false;
        }
    }

    return true;
}

void switchToBT()
{
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);

    Serial.println("WiFi disabled, restarting BT\n");

    SerialBT.begin(BT_DEVICE);
}