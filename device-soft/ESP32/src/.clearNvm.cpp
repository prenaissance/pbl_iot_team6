#include <nvs_flash.h>
#include <Preferences.h>

#define DEVICE_PARAM_NAMESPACE "device-param"
#define DEVICE_ID_SELECTOR "device-id"

Preferences preferences;

void setup()
{
    Serial.begin(9600);
    preferences.begin(DEVICE_PARAM_NAMESPACE, false);
    preferences.clear();
    nvs_flash_erase(); // erase the NVS partition and...
    nvs_flash_init();  // initialize the NVS partition.
    delay(5000);
}

void loop()
{
    Serial.println("Done");
    delay(1000);
}