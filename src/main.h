#include <Arduino.h>
#include <TimerOne.h>
#include "upsw.h"
#include "config.h"

/**
 * calculate the battery voltage from a analog reading)
 * 
 * @param analogReading the analogRead() value on the battery sense ping
 * @return the battery voltage, in volts
 */
double calculateBatteryVoltage(int analogReading);

/**
 * handler function for hearthbeat
 */
void hearthbeatHandler();
