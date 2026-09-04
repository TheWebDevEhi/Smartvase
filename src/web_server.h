#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>

class WebServerManager {
public:
    static void init();
    static void update(); // for AP timeout check
    static void handleClientActive();

private:
    static void setupEndpoints();
    static uint32_t boot_time;
    static bool ap_active;
    static uint32_t last_api_call_ms;
    static bool should_reboot;
    static uint32_t reboot_time;
};

#endif // WEB_SERVER_H
