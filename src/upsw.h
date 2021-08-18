#include <Arduino.h>
#include "pdlib/HIDPowerDevice.h"
#include "config.h"

/**
 * initialize the UPS
 */
void UPS_init(void);

/**
 * update the status of the ups
 * 
 * @param isCharging is the ups currently charging?
 * @param isAcPresent is the ups currently on AC power?
 * @param doRequestShutdown should the host shut down (SHUTDOWN_REQUEST)?
 * @param isBatteryPresent is a battery installed in the UPS?
 * @param remainingCapacity remaining capacity of the UPS, in %
 * @param remainingRuntime how many seconds until the battery of the UPS is empty
 */
void UPS_update(bool isCharging, bool isAcPresent, bool doRequestShutdown, bool isBatteryPresent, uint16_t remainingCapacity, uint16_t remainingRuntime);

/**
 * send a report to the host (with the current values) on demand
 * 
 * @param isDischarging is the ups currently discharging?
 * @param force force- send the report
 * @param didSend was a report sent to the host?
 * 
 * @return the result code of the sendReport call. 0 if success or not send, non- zero if failed
 */
int UPS_sendReport(bool isDischarging, bool force, bool *didSend);
