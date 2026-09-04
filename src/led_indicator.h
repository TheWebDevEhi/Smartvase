#ifndef LED_INDICATOR_H
#define LED_INDICATOR_H

#include <Arduino.h>

class LedIndicator {
public:
    static void init();
    static void update();
    
    static void setSystemHealthy(bool healthy);
    static void setWifiState(bool active);
    
private:
    static bool sys_healthy;
    static bool wifi_active;
    static uint32_t blink_ms;
};

#endif // LED_INDICATOR_H
