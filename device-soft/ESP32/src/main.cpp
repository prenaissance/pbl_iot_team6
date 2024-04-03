#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "drivers/lcdDriver.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
static LcdDriver ld(&lcd);

void setup(){
    Serial.begin(9600);

    lcd.init(); lcd.backlight();

    for (int i = 0; i < 5; i++) {
        char line2[16];
        sprintf(line2, "MSG. NUM. %d", i);
        ld.accessQueue()->enqueue(LcdMsg("THIS IS A TEST", line2, 1, 1000));
    }

    ld.accessQueue()->enqueue(LcdMsg("THIS IS A MSG.", "OF A > PRIOR.", 5, 2000));
}

void loop(){
    ld.update();
    delay(1);
}