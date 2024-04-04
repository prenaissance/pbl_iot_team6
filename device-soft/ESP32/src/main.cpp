#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "drivers/lcdDriver.h"
#include "drivers/rfidDriver.h"
#include "drivers/pickupSys.h"

#define BAUD_RATE 9600
#define SS_PIN 5
#define RST_PIN 22

LiquidCrystal_I2C lcd(0x27, 16, 2);

static PiezoBuzzerDriver pbd(10);
static IRSensDriver irsd(11);
static LcdDriver ld(&lcd);
static RfidDriverManager rm;

void setup(){
    Serial.begin(BAUD_RATE);

    // lcd.init(); lcd.backlight();
}

void loop(){
}