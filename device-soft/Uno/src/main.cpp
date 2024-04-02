#include <Arduino.h>

#include "drivers/stateLedDriver.h"
#include "drivers/dispenserStepperDriver.h"

#define BAUD_RATE 9600

static StateLedDriverManager lm;
static DispenserStepperDriverManager sm;

void setup() {
    Serial.begin(BAUD_RATE);
    sm.init(DispenserStepperDriver(9, 10, 11, 12), 0);
    sm.initRevolution(0);
}

void loop() {
    delay(2);
    sm.manageRevolutions();
}