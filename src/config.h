#ifndef config_h
#define config_h

/**
 * enable CDC (Serial) of the leonardo. this has to be changed in the core too, see README for that
 */
#define ENABLE_CDC

/**
 * disable shutdown requests to the host, useful when debugging
 */
#define DO_NOT_SHUTDOWN

/**
 * pin the hearthbeat LED is attached to.
 */
#define HEARTHBEAT_LED 5

/**
 * pin the communication failure / error LED is attached to
 */
#define COMM_FAIL_LED 10

/**
 * pin used for AC presence sense
 * (this depends on your hardware)
 */
#define AC_SENSE A3

/**
 * analog pin used to sense the battery voltage
 * (this depends on your hardware)
 */
#define BATT_SENSE A2

/**
 * interval the status of the UPS is refreshed at, in ms
 */
#define SENSE_INTERVAL 1000

/**
 * the minimum time between reports to the host computer, in ms
 */
#define MIN_UPDATE_INTERVAL 10000

/**
 * lowest possible battery voltage (~0%)
 */
#define BAT_MIN_VOLTAGE 10.5

/**
 * highest possible battery voltage (~100%)
 */
#define BAT_MAX_VOLTAGE 14.5

/**
 * voltage measured when no battery is installed
 * (depends on your hardware, ...)
 */
#define BAT_NO_VOLTAGE 7.0

/**
 * device chemistry string, see HID spec for this
 */
const char STRING_DEVICECHEMISTRY[] PROGMEM = "PbAc";

/**
 * OEM/Vendor name string
 */
const char STRING_OEMVENDOR[] PROGMEM = "ArduinoUPS";

/**
 * display name of the serial interface device
 */
const char STRING_SERIAL[] PROGMEM = "UPSDbg";

/**
 * the average charge time, in seconds
 */
#define AVG_CHARGE_TIME 3600 // 1 hour

/**
 * the average discharge time, in seconds
 */
#define AVG_DISCHARGE_TIME 300 // 5 minutes

/**
 * the minimum time remaining, in seconds. 
 * if this time is reached, host computers will shutdown 
 */
#define MIN_REMAINING_TIME 600

/**
 * design capacity. because units are hardcoded to percent, this does not really make sense to change.
 * but you can change the units in upsw.cpp in UPS_init()
 */
#define DESIGN_CAPACITY 100

/**
 * the capacity when the battery is fully charged, in percent
 */
#define FULL_CHARGE_CAPACITY 100

/**
 * the capacity the host should warn the battery is running low, in percent
 */
#define WARN_CAPACITY 25

/**
 * the minimum allowed capacity. the host should shut down when this capacity is reached. in percent
 */
#define MIN_CAPACITY 10

/**
 * 'measured' line voltage, in V.
 * Set this to the line voltage in your country
 * there is currently no way implemented to change this, but it is generally possible
 */
#define VOLTAGE 240

#endif