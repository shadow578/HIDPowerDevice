#include "runtimeEstimate.h"

uint32_t dischargeStartTime = 0;

uint16_t Runtime_calculateEstimate(uint16_t currentCapacity, bool isDischarging)
{
    /**
     * this uses a simple algorithm where we actually just count the time singe the discharge started
     * and substract that from the average time for a full discharge.
     * this does not take into account the power draw of the host or anything like that
     */ 

    if (isDischarging)
    {
        // save time of discharge time
        if (dischargeStartTime == 0)
        {
            dischargeStartTime = millis();
        }

        // calculate time since discharge start
        // in seconds
        uint16_t timeSinceDischargeStart = (millis() - dischargeStartTime) / 1000;

        // substract from average total discharge time
        return AVG_DISCHARGE_TIME - timeSinceDischargeStart;
    }
    else
    {
        // reset discharge start time
        dischargeStartTime = 0;

        // is not discharging, assume full
        return AVG_DISCHARGE_TIME;
    }
}
