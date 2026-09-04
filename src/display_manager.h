#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>

class DisplayManager {
public:
    static void init();
    static void update();
    static void turnOn();
    static void turnOff();
    
private:
    static bool is_on;
    static uint32_t last_activity_ms;
};

#endif // DISPLAY_MANAGER_H
