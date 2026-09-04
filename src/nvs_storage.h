#ifndef NVS_STORAGE_H
#define NVS_STORAGE_H

#include <Arduino.h>
#include <Preferences.h>
#include "config.h"

struct PumpSchedule {
    uint32_t next_trigger_unix;
    uint16_t duration_seconds;
    uint8_t  repeat_hours;
    bool     enabled;
};

class NVSStorage {
public:
    static void init();
    
    static uint16_t getCalDry();
    static void setCalDry(uint16_t val);
    
    static uint16_t getCalWet();
    static void setCalWet(uint16_t val);
    
    static uint8_t getPumpMaxDuration();
    static void setPumpMaxDuration(uint8_t val);
    
    static uint8_t getMoistureThreshold();
    static void setMoistureThreshold(uint8_t val);
    
    static String getWifiSSID();
    static void setWifiSSID(const String& ssid);
    
    static String getWifiPass();
    static void setWifiPass(const String& pass);
    
    static String getAdminPass();
    static void setAdminPass(const String& pass);
    
    static bool getSchedule(PumpSchedule& sched);
    static void setSchedule(const PumpSchedule& sched);

private:
    static Preferences prefs;
};

#endif // NVS_STORAGE_H
