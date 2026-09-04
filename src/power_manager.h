#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include <Arduino.h>

enum class WakeCause {
    POWER_ON,
    PIR_MOTION,
    TIMER_WAKE,
    UNKNOWN
};

class PowerManager {
public:
    static void init();
    static WakeCause getWakeCause();
    static uint8_t getBatteryPercent();
    static float getBatteryVoltage();
    static void goToSleep(uint32_t sleep_seconds);
    
private:
    static float readBatteryVoltageRaw();
    static WakeCause currentWakeCause;
};

#endif // POWER_MANAGER_H
