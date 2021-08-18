#include "upsw.h"

uint16_t currentPresentStatus = 0, oldPresentStatus = 0;
uint16_t currentRemainingCharge = 0, oldRemainingCharge = 0;
uint16_t currentRuntimeRemaining = 0, oldRuntimeRemaining = 0;

void UPS_init()
{
    // init HID power device
    PowerDevice.begin();

    // current status
    PowerDevice.setFeature(HID_PD_PRESENTSTATUS, &currentPresentStatus, sizeof(currentPresentStatus));
    PowerDevice.setFeature(HID_PD_REMAININGCAPACITY, &currentRemainingCharge, sizeof(currentRemainingCharge));
    PowerDevice.setFeature(HID_PD_RUNTIMETOEMPTY, &currentRuntimeRemaining, sizeof(currentRuntimeRemaining));

    /* #region physical parameters */
    const uint16_t averageTimeUntilFull = AVG_CHARGE_TIME;
    PowerDevice.setFeature(HID_PD_AVERAGETIME2FULL, &averageTimeUntilFull, sizeof(averageTimeUntilFull));

    const uint16_t averageTimeUntilEmpty = AVG_DISCHARGE_TIME;
    PowerDevice.setFeature(HID_PD_AVERAGETIME2EMPTY, &averageTimeUntilEmpty, sizeof(averageTimeUntilEmpty));

    const uint16_t remainingTimeLImit = MIN_REMAINING_TIME;
    PowerDevice.setFeature(HID_PD_REMAINTIMELIMIT, &remainingTimeLImit, sizeof(remainingTimeLImit));

    const int16_t delayUntilReboot = -1;
    PowerDevice.setFeature(HID_PD_DELAYBE4REBOOT, &delayUntilReboot, sizeof(delayUntilReboot));

    const int16_t delayUntilShutdown = -1;
    PowerDevice.setFeature(HID_PD_DELAYBE4SHUTDOWN, &delayUntilShutdown, sizeof(delayUntilShutdown));

    const byte rechargeable = 1;
    PowerDevice.setFeature(HID_PD_RECHARGEABLE, &rechargeable, sizeof(rechargeable));

    const uint16_t configVoltage = VOLTAGE;
    PowerDevice.setFeature(HID_PD_CONFIGVOLTAGE, &configVoltage, sizeof(configVoltage));

    uint16_t currentVoltage = VOLTAGE;
    PowerDevice.setFeature(HID_PD_VOLTAGE, &currentVoltage, sizeof(currentVoltage));
    /* #endregion */

    /* #region ACPI compliance parameters */
    const byte designCapacity = DESIGN_CAPACITY;
    PowerDevice.setFeature(HID_PD_DESIGNCAPACITY, &designCapacity, sizeof(designCapacity));

    const byte fullChargeCapacity = FULL_CHARGE_CAPACITY;
    PowerDevice.setFeature(HID_PD_FULLCHRGECAPACITY, &fullChargeCapacity, sizeof(fullChargeCapacity));

    const byte warnCapacityLimit = WARN_CAPACITY; // warning at 10%
    PowerDevice.setFeature(HID_PD_WARNCAPACITYLIMIT, &warnCapacityLimit, sizeof(warnCapacityLimit));

    const byte remainingCapacityLimit = MIN_CAPACITY; // low at 5%
    PowerDevice.setFeature(HID_PD_REMNCAPACITYLIMIT, &remainingCapacityLimit, sizeof(remainingCapacityLimit));

    const byte capacityGranularity1 = 1;
    PowerDevice.setFeature(HID_PD_CPCTYGRANULARITY1, &capacityGranularity1, sizeof(capacityGranularity1));

    const byte capacityGranularity2 = 1;
    PowerDevice.setFeature(HID_PD_CPCTYGRANULARITY2, &capacityGranularity2, sizeof(capacityGranularity2));
    /* #endregion */

    /* #region OEM strings */
    const byte deviceChemistry = IDEVICECHEMISTRY;
    PowerDevice.setStringFeature(HID_PD_IDEVICECHEMISTRY, &deviceChemistry, STRING_DEVICECHEMISTRY);

    const byte oemVendorName = IOEMVENDOR;
    PowerDevice.setStringFeature(HID_PD_IOEMINFORMATION, &oemVendorName, STRING_OEMVENDOR);
    /* #endregion */

    // additional settings
    const byte capacityMode = 2; // units are in %
    PowerDevice.setFeature(HID_PD_CAPACITYMODE, &capacityMode, sizeof(capacityMode));

    const byte audibleAlarmControl = 2; // 1 - Disabled, 2 - Enabled, 3 - Muted
    PowerDevice.setFeature(HID_PD_AUDIBLEALARMCTRL, &audibleAlarmControl, sizeof(audibleAlarmControl));
}

void UPS_update(bool isCharging, bool isAcPresent, bool doRequestShutdown, bool isBatteryPresent, uint16_t remainingCapacity, uint16_t remainingRuntime)
{
    // update values
    currentRemainingCharge = remainingCapacity;
    currentRuntimeRemaining = remainingRuntime;

    // overwrite shutdown request
#ifdef DO_NOT_SHUTDOWN
    doRequestShutdown = false;
#endif

    /* #region present status flags */
    /* #region charging flag */
    if (isCharging)
        bitSet(currentPresentStatus, PRESENTSTATUS_CHARGING);
    else
        bitClear(currentPresentStatus, PRESENTSTATUS_CHARGING);
    /* #endregion */

    /* #region AC Present flag */
    if (isAcPresent)
        bitSet(currentPresentStatus, PRESENTSTATUS_ACPRESENT);
    else
        bitClear(currentPresentStatus, PRESENTSTATUS_ACPRESENT);
    /* #endregion */

    /* #region fully charged flag */
    if (currentRemainingCharge == FULL_CHARGE_CAPACITY)
        bitSet(currentPresentStatus, PRESENTSTATUS_FULLCHARGE);
    else
        bitClear(currentPresentStatus, PRESENTSTATUS_FULLCHARGE);
    /* #endregion */

    /* #region discharging flag */
    if (!isCharging)
        bitSet(currentPresentStatus, PRESENTSTATUS_DISCHARGING);
    else
        bitClear(currentPresentStatus, PRESENTSTATUS_DISCHARGING);
    /* #endregion */

    /* #region runtime expired flag */
    if (!isCharging && currentRuntimeRemaining < MIN_REMAINING_TIME)
        bitSet(currentPresentStatus, PRESENTSTATUS_RTLEXPIRED);
    else
        bitClear(currentPresentStatus, PRESENTSTATUS_RTLEXPIRED);
    /* #endregion */

    /* #region shutdown request flag */
    if (doRequestShutdown)
        bitSet(currentPresentStatus, PRESENTSTATUS_SHUTDOWNREQ);
    else
        bitClear(currentPresentStatus, PRESENTSTATUS_SHUTDOWNREQ);
    /* #endregion */

    /* #region shutdown imminent flag */
    if ((currentPresentStatus & (1 << PRESENTSTATUS_SHUTDOWNREQ)) ||
        (currentPresentStatus & (1 << PRESENTSTATUS_RTLEXPIRED)))
    {
        bitSet(currentPresentStatus, PRESENTSTATUS_SHUTDOWNIMNT);
    }
    else
        bitClear(currentPresentStatus, PRESENTSTATUS_SHUTDOWNIMNT);
    /* #endregion */

    /* #region battery presence flag */
    if (isBatteryPresent)
        bitSet(currentPresentStatus, PRESENTSTATUS_BATTPRESENT);
    else
        bitClear(currentPresentStatus, PRESENTSTATUS_BATTPRESENT);
    /* #endregion */
    /* #endregion */
}

int UPS_sendReport(bool isDischarging, bool force, bool *didSend)
{
    int result = 0;
    *didSend = false;
    if (force || (currentPresentStatus != oldPresentStatus) || (currentRemainingCharge != oldRemainingCharge) || (currentRuntimeRemaining != oldRuntimeRemaining))
    {
        // send reports
        //TODO using OR instead of just last report, does this work correctly??
        result |= PowerDevice.sendReport(HID_PD_REMAININGCAPACITY, &currentRemainingCharge, sizeof(currentRemainingCharge));
        result |= PowerDevice.sendReport(HID_PD_RUNTIMETOEMPTY, &currentRuntimeRemaining, sizeof(currentRuntimeRemaining));
        result |= PowerDevice.sendReport(HID_PD_PRESENTSTATUS, &currentPresentStatus, sizeof(currentPresentStatus));

        // update values for next compare
        oldPresentStatus = currentPresentStatus;
        oldRemainingCharge = currentRemainingCharge;
        oldRuntimeRemaining = currentRuntimeRemaining;

        // set send flag
        *didSend = true;
    }

    return result;
}
