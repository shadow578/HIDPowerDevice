#include "main.h"

#define ENABLE_CDC

#define DO_NOT_SHUTDOWN

#define HEARTHBEAT_LED 5          //PIN
#define COMM_FAIL_LED 10          //PIN
#define AC_SENSE A3               //PIN
#define BATT_SENSE A2             //PIN
#define SENSE_INTERVAL 1000       //ms
#define MIN_UPDATE_INTERVAL 10000 //ms

#define BAT_MIN_VOLTAGE 10.5 //V
#define BAT_MAX_VOLTAGE 14.5 //V
#define BAT_NO_VOLTAGE 7     //V

const char STRING_DEVICECHEMISTRY[] PROGMEM = "PbAc";
const char STRING_OEMVENDOR[] PROGMEM = "ArduinoUPS";
const char STRING_SERIAL[] PROGMEM = "UPSDbg";
#define AVG_CHARGE_TIME 120 * 60   //s
#define AVG_DISCHARGE_TIME 30 * 60 //s
#define MIN_REMAINING_TIME 600     //s
#define DESIGN_CAPACITY 100        //%
#define FULL_CHARGE_CAPACITY 100   //%
#define WARN_CAPACITY 25           //%
#define MIN_CAPACITY 10            //%
#define VOLTAGE 240 * 1000         //mV; dummy value

bool ledState = false;
void blinkLed()
{
  ledState = !ledState;
  digitalWrite(HEARTHBEAT_LED, ledState);
}

void setup()
{
  // init HID power device
  PowerDevice.begin();

  // setup debugging using serial (if CDC enabled)
#ifdef ENABLE_CDC
  Serial.begin(115200);
  PowerDevice.setSerial(STRING_SERIAL);
  PowerDevice.setOutput(Serial);
#endif

  // initialize UPS
  initUPS();

  // setup pins
  pinMode(HEARTHBEAT_LED, OUTPUT);
  pinMode(COMM_FAIL_LED, OUTPUT);
  pinMode(AC_SENSE, INPUT);
  pinMode(BATT_SENSE, INPUT);

  // blink led to show we are working
  Timer1.initialize(500000);
  Timer1.attachInterrupt(blinkLed);
}

uint32_t lastUpdateMillis = 0;
double ubatReal;
void loop()
{
  // measure
  bool acPresent = !digitalRead(AC_SENSE);
  int ubatReading = analogRead(BATT_SENSE);

  // adjust ubat to real voltage (volts)
  double ubat = approxBatVoltage(ubatReading);

  // get percent value for battery voltage
  int ubatPercent = (int)round(((ubat - BAT_MIN_VOLTAGE) * 100) / (BAT_MAX_VOLTAGE - BAT_MIN_VOLTAGE));
  bool ubatLow = ubatPercent <= MIN_CAPACITY;
  bool noBat = ubat <= BAT_NO_VOLTAGE;

  // ignore low battery if on ac power
  if (acPresent && ubatLow)
  {
    ubatLow = false;
    ubatPercent = WARN_CAPACITY;
  }

  // clamp battery percent to 0-100
  if (ubatPercent < 0)
    ubatPercent = 0;
  if (ubatPercent > 100)
    ubatPercent = 100;

  // update ups status and send report
  bool forceUpdate = (millis() - lastUpdateMillis) > MIN_UPDATE_INTERVAL;
  updateUPS(acPresent, acPresent, ubatLow, noBat, ubatPercent, 0);
  bool commFail = sendReport(!acPresent, forceUpdate) < 0;
  if (commFail)
  {
    // send fail
    digitalWrite(COMM_FAIL_LED, HIGH);
  }
  else
  {
    //send ok
    digitalWrite(COMM_FAIL_LED, LOW);
  }

// debug infos
#ifdef ENABLE_CDC
  Serial.print("ac: ");
  Serial.print(acPresent);
  Serial.print("; ubat: ");
  Serial.print(ubat);
  Serial.print("V (");
  Serial.print(ubatReading);
  Serial.print(" 1/1024, REAL: ");
  Serial.print(ubatReal);
  Serial.print("V) ");
  Serial.print(", ");
  Serial.print(ubatPercent);
  Serial.print("%, is_low: ");
  Serial.print(ubatLow);
  Serial.print(", no_battery: ");
  Serial.print(noBat);
  Serial.print("; force_update: ");
  Serial.print(forceUpdate);
  Serial.print("; usb_comm_fail: ");
  Serial.println(commFail);
#endif

  // wait before next measure
  delay(SENSE_INTERVAL);

  //*********** Measurements Unit ****************************
  //bool bCharging = digitalRead(4);
  //bool bACPresent = bCharging;    // TODO - replace with sensor
  //bool bDischarging = !bCharging; // TODO - replace with sensor
  //int iA7 = analogRead(A7);       // TODO - this is for debug only. Replace with charge estimation
  //
  //iRemaining = (byte)(round((float)100 * iA7 / 1024));
  //iRunTimeToEmpty = (uint16_t)round((float)iAvgTimeToEmpty * iRemaining / 100);
}

/**
 * get a approximate voltage from a analogRead() reading.
 * A optocoupler is used to isolate the battery voltage from the usb interface.
 * Since the (led) voltage stays in a range where the optocoupler circuit acts linear, we can just calculate the voltage using
 * Usense = m * Ubat + b
 * OR
 * Ubat = (Usense - b) / m
 * 
 * For my configuration, i get the values
 * m = 352.5
 * b = -550
 * 
 * As my battery sense voltage is scaled down before the optocoupler, i have to correct for that too (factor 0.25)
 */
double approxBatVoltage(int analogReading)
{
  // linear approximation
  ubatReal = (analogReading + 550) / 352.5;

  // adjust scale
  return ubatReal / 0.25;
}

#pragma region HID
const byte bDeviceChemistry = IDEVICECHEMISTRY;
const byte bOEMVendor = IOEMVENDOR;

uint16_t iPresentStatus = 0, iPreviousStatus = 0;

byte bRechargable = 1;
byte bCapacityMode = 2; // units are in %%

// Physical parameters
const uint16_t iConfigVoltage = VOLTAGE;
uint16_t iVoltage = VOLTAGE;
uint16_t iRunTimeToEmpty = 0, iPrevRunTimeToEmpty = 0;
uint16_t iAvgTimeToFull = AVG_CHARGE_TIME;
uint16_t iAvgTimeToEmpty = AVG_DISCHARGE_TIME;
uint16_t iRemainTimeLimit = MIN_REMAINING_TIME;
int16_t iDelayBe4Reboot = -1;
int16_t iDelayBe4ShutDown = -1;

byte iAudibleAlarmCtrl = 2; // 1 - Disabled, 2 - Enabled, 3 - Muted

// Parameters for ACPI compliancy
const byte iDesignCapacity = DESIGN_CAPACITY;
byte iWarnCapacityLimit = WARN_CAPACITY; // warning at 10%
byte iRemnCapacityLimit = MIN_CAPACITY;  // low at 5%
const byte bCapacityGranularity1 = 1;
const byte bCapacityGranularity2 = 1;
byte iFullChargeCapacity = FULL_CHARGE_CAPACITY;

uint16_t iRemaining = 0, iPrevRemaining = 0;

void initUPS()
{
  PowerDevice.setFeature(HID_PD_PRESENTSTATUS, &iPresentStatus, sizeof(iPresentStatus));

  PowerDevice.setFeature(HID_PD_RUNTIMETOEMPTY, &iRunTimeToEmpty, sizeof(iRunTimeToEmpty));
  PowerDevice.setFeature(HID_PD_AVERAGETIME2FULL, &iAvgTimeToFull, sizeof(iAvgTimeToFull));
  PowerDevice.setFeature(HID_PD_AVERAGETIME2EMPTY, &iAvgTimeToEmpty, sizeof(iAvgTimeToEmpty));
  PowerDevice.setFeature(HID_PD_REMAINTIMELIMIT, &iRemainTimeLimit, sizeof(iRemainTimeLimit));
  PowerDevice.setFeature(HID_PD_DELAYBE4REBOOT, &iDelayBe4Reboot, sizeof(iDelayBe4Reboot));
  PowerDevice.setFeature(HID_PD_DELAYBE4SHUTDOWN, &iDelayBe4ShutDown, sizeof(iDelayBe4ShutDown));

  PowerDevice.setFeature(HID_PD_RECHARGEABLE, &bRechargable, sizeof(bRechargable));
  PowerDevice.setFeature(HID_PD_CAPACITYMODE, &bCapacityMode, sizeof(bCapacityMode));
  PowerDevice.setFeature(HID_PD_CONFIGVOLTAGE, &iConfigVoltage, sizeof(iConfigVoltage));
  PowerDevice.setFeature(HID_PD_VOLTAGE, &iVoltage, sizeof(iVoltage));

  PowerDevice.setStringFeature(HID_PD_IDEVICECHEMISTRY, &bDeviceChemistry, STRING_DEVICECHEMISTRY);
  PowerDevice.setStringFeature(HID_PD_IOEMINFORMATION, &bOEMVendor, STRING_OEMVENDOR);

  PowerDevice.setFeature(HID_PD_AUDIBLEALARMCTRL, &iAudibleAlarmCtrl, sizeof(iAudibleAlarmCtrl));

  PowerDevice.setFeature(HID_PD_DESIGNCAPACITY, &iDesignCapacity, sizeof(iDesignCapacity));
  PowerDevice.setFeature(HID_PD_FULLCHRGECAPACITY, &iFullChargeCapacity, sizeof(iFullChargeCapacity));
  PowerDevice.setFeature(HID_PD_REMAININGCAPACITY, &iRemaining, sizeof(iRemaining));
  PowerDevice.setFeature(HID_PD_WARNCAPACITYLIMIT, &iWarnCapacityLimit, sizeof(iWarnCapacityLimit));
  PowerDevice.setFeature(HID_PD_REMNCAPACITYLIMIT, &iRemnCapacityLimit, sizeof(iRemnCapacityLimit));
  PowerDevice.setFeature(HID_PD_CPCTYGRANULARITY1, &bCapacityGranularity1, sizeof(bCapacityGranularity1));
  PowerDevice.setFeature(HID_PD_CPCTYGRANULARITY2, &bCapacityGranularity2, sizeof(bCapacityGranularity2));
}

void updateUPS(bool charging, bool acPresent, bool reqShutdown, bool batteryPresent, uint16_t remaining, uint16_t runtimeToEmpty)
{
  iRemaining = remaining;
  iRunTimeToEmpty = runtimeToEmpty;

// overwrite shutdown request
#ifdef DO_NOT_SHUTDOWN
  reqShutdown = false;
#endif

  // Charging
  if (charging)
    bitSet(iPresentStatus, PRESENTSTATUS_CHARGING);
  else
    bitClear(iPresentStatus, PRESENTSTATUS_CHARGING);

  // AC present
  if (acPresent)
    bitSet(iPresentStatus, PRESENTSTATUS_ACPRESENT);
  else
    bitClear(iPresentStatus, PRESENTSTATUS_ACPRESENT);

  if (iRemaining == iFullChargeCapacity)
    bitSet(iPresentStatus, PRESENTSTATUS_FULLCHARGE);
  else
    bitClear(iPresentStatus, PRESENTSTATUS_FULLCHARGE);

  // Discharging
  if (!charging)
  {
    bitSet(iPresentStatus, PRESENTSTATUS_DISCHARGING);

    if (iRunTimeToEmpty < iRemainTimeLimit)
      bitSet(iPresentStatus, PRESENTSTATUS_RTLEXPIRED);
    else
      bitClear(iPresentStatus, PRESENTSTATUS_RTLEXPIRED);
  }
  else
  {
    bitClear(iPresentStatus, PRESENTSTATUS_DISCHARGING);
    bitClear(iPresentStatus, PRESENTSTATUS_RTLEXPIRED);
  }

  // Shutdown requested
  if (reqShutdown)
    bitSet(iPresentStatus, PRESENTSTATUS_SHUTDOWNREQ);
  else
    bitClear(iPresentStatus, PRESENTSTATUS_SHUTDOWNREQ);

  // Shutdown imminent
  if ((iPresentStatus & (1 << PRESENTSTATUS_SHUTDOWNREQ)) ||
      (iPresentStatus & (1 << PRESENTSTATUS_RTLEXPIRED)))
  {
    bitSet(iPresentStatus, PRESENTSTATUS_SHUTDOWNIMNT);
  }
  else
    bitClear(iPresentStatus, PRESENTSTATUS_SHUTDOWNIMNT);

  if (batteryPresent)
    bitSet(iPresentStatus, PRESENTSTATUS_BATTPRESENT);
  else
    bitClear(iPresentStatus, PRESENTSTATUS_BATTPRESENT);
}

int sendReport(bool discharging, bool force)
{
  if (force || (iPresentStatus != iPreviousStatus) || (iRemaining != iPrevRemaining) || (iRunTimeToEmpty != iPrevRunTimeToEmpty))
  {
    PowerDevice.sendReport(HID_PD_REMAININGCAPACITY, &iRemaining, sizeof(iRemaining));
    PowerDevice.sendReport(HID_PD_RUNTIMETOEMPTY, &iRunTimeToEmpty, sizeof(iRunTimeToEmpty));
    int iRes = PowerDevice.sendReport(HID_PD_PRESENTSTATUS, &iPresentStatus, sizeof(iPresentStatus));

    iPreviousStatus = iPresentStatus;
    iPrevRemaining = iRemaining;
    iPrevRunTimeToEmpty = iRunTimeToEmpty;

    lastUpdateMillis = millis();
#ifdef ENABLE_CDC
    Serial.println("sendReport");
#endif
    return iRes;
  }

  return 0;
}

#pragma endregion
