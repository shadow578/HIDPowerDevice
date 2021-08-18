#include <Arduino.h>
#include "config.h"

/**
 * estimate the remaining runtime of the UPS
 * 
 * @param currentCapacity the current battery capacity, in percent
 * @param isDischarging is the UPS currently discharging?
 */
uint16_t Runtime_calculateEstimate(uint16_t currentCapacity, bool isDischarging);
