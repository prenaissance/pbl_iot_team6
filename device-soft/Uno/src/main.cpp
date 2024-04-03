#include <Arduino.h>

#include "drivers/stateLedDriver.h"
#include "drivers/dispenserStepperDriver.h"
#include "drivers/rfidDriver.h"

#define BAUD_RATE 9600
#define SS_PIN 5
#define RST_PIN 22

static StateLedDriverManager lm;
static DispenserStepperDriverManager sm;
static RfidDriverManager rm;

void setup()
{
    Serial.begin(BAUD_RATE);
    rm.init(&RfidDriver(SS_PIN, RST_PIN));
    sm.init(DispenserStepperDriver(9, 10, 11, 12), 0);
    sm.initRevolution(0);
}

void loop()
{
    delay(2);
    rm.manageUid();
    sm.manageRevolutions();
}