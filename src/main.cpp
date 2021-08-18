#include "main.h"

void setup()
{
  // setup pins
  pinMode(HEARTHBEAT_LED, OUTPUT);
  pinMode(COMM_FAIL_LED, OUTPUT);
  pinMode(AC_SENSE, INPUT);
  pinMode(BATT_SENSE, INPUT);

  // setup debugging using serial (if CDC enabled)
#ifdef ENABLE_CDC
  Serial.begin(115200);
  PowerDevice.setSerial(STRING_SERIAL);
  PowerDevice.setOutput(Serial);
#endif

  // initialize UPS
  UPS_init();

  // setup hearthbeat
  Timer1.initialize(500000);
  Timer1.attachInterrupt(hearthbeatHandler);
}

uint32_t lastReportTime = 0;
void loop()
{
  // read inputs
  bool isAcPresent = !digitalRead(AC_SENSE);
  int batteryVoltageReading = analogRead(BATT_SENSE);

  // calculate real voltage from raw battery reading
  double batteryVoltage = calculateBatteryVoltage(batteryVoltageReading);

  // get percent value for battery voltage
  int batteryPercent = (int)round(((batteryVoltage - BAT_MIN_VOLTAGE) * 100) / (BAT_MAX_VOLTAGE - BAT_MIN_VOLTAGE));
  bool isBatteryLow = batteryPercent <= MIN_CAPACITY;
  bool isBatteryPresent = batteryVoltage <= BAT_NO_VOLTAGE;

  // ignore low battery if on ac power
  // and overwrite capacity to minimum that should not cause issues
  if (isAcPresent && isBatteryLow)
  {
    isBatteryLow = false;
    batteryPercent = WARN_CAPACITY + 1;
  }

  // clamp battery percent to 0-100
  if (batteryPercent < 0)
    batteryPercent = 0;
  if (batteryPercent > 100)
    batteryPercent = 100;

  // approximate time until discharge
  uint16_t timeUntilDischarged = Runtime_calculateEstimate(batteryPercent, !isAcPresent);

  // update UPS values
  UPS_update(isAcPresent, isAcPresent, isBatteryLow, isBatteryPresent, batteryPercent, timeUntilDischarged);

  // send report to the host
  bool forceUpdate = (millis() - lastReportTime) > MIN_UPDATE_INTERVAL;
  bool didSendReport = false;
  int reportResult = UPS_sendReport(!isAcPresent, forceUpdate, &didSendReport);
  if (reportResult < 0)
  {
    // send failure
    digitalWrite(COMM_FAIL_LED, HIGH);
  }
  else
  {
    // send OK
    digitalWrite(COMM_FAIL_LED, LOW);
  }

  // update time of last report
  if (didSendReport)
  {
    lastReportTime = millis();
  }

// debug infos
#ifdef ENABLE_CDC
  Serial.print("ac: ");
  Serial.print(isAcPresent);
  Serial.print("; ubat: ");
  Serial.print(batteryVoltage);
  Serial.print("V (");
  Serial.print(batteryVoltageReading);
  Serial.print(" 1/1024), ");
  Serial.print(batteryPercent);
  Serial.print("%, is_low: ");
  Serial.print(isBatteryLow);
  Serial.print(", no_battery: ");
  Serial.print(isBatteryPresent);
  Serial.print("; force_update: ");
  Serial.print(forceUpdate);
  Serial.print("; hid_report_result: ");
  Serial.println(reportResult);
#endif

  // wait before next measure
  delay(SENSE_INTERVAL);
}

double calculateBatteryVoltage(int analogReading)
{
  /*
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

  // linear approximation
  const double ubatReal = (analogReading + 550) / 352.5;

  // adjust scale
  return ubatReal / 0.25;
}

bool hearthbeatLedState = false;
void hearthbeatHandler()
{
  hearthbeatLedState = !hearthbeatLedState;
  digitalWrite(HEARTHBEAT_LED, hearthbeatLedState);
}
