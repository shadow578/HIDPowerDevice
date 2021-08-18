#include <Arduino.h>
#include <TimerOne.h>
#include "HIDPowerDevice.h"

void blinkLed();
void setup();
void loop();
double approxBatVoltage(int analogReading);
void initUPS(void);
void updateUPS(bool charging, bool acPresent, bool reqShutdown, bool batteryPresent, uint16_t remaining, uint16_t runtimeToEmpty);
int sendReport(bool discharging, bool force);
