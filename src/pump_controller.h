#ifndef PUMP_CONTROLLER_H
#define PUMP_CONTROLLER_H

#include <Arduino.h>

class PumpController {
public:
    static void init();
    static void update(); // Called in loop() for non-blocking timing
    
    // Attempt to start pump (returns false if blocked by safety)
    static bool startPump(uint8_t duration_sec, bool override_safety = false);
    static void stopPump();
    
    static bool isPumpRunning();
    static uint32_t getLastPumpTime();

private:
    static bool pump_running;
    static uint32_t pump_start_ms;
    static uint32_t pump_duration_ms;
    static uint32_t last_pump_time_unix; // For cooldown/record
};

#endif // PUMP_CONTROLLER_H
