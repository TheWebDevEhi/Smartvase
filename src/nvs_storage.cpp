#include "nvs_storage.h"

Preferences NVSStorage::prefs;

void NVSStorage::init() {
    prefs.begin(NVS_NAMESPACE, false); // false = read/write mode
    // Initialize default values if not present (robust method avoiding isKey)
    prefs.putUShort("cal_dry", prefs.getUShort("cal_dry", 4095));
    prefs.putUShort("cal_wet", prefs.getUShort("cal_wet", 1500));
    prefs.putUChar("pump_max", prefs.getUChar("pump_max", MAX_PUMP_DURATION_SEC));
    prefs.putUChar("moist_thr", prefs.getUChar("moist_thr", 30));
    prefs.putString("admin_pass", prefs.getString("admin_pass", "admin"));
}

uint16_t NVSStorage::getCalDry() { return prefs.getUShort("cal_dry", 4095); }
void NVSStorage::setCalDry(uint16_t val) { prefs.putUShort("cal_dry", val); }

uint16_t NVSStorage::getCalWet() { return prefs.getUShort("cal_wet", 1500); }
void NVSStorage::setCalWet(uint16_t val) { prefs.putUShort("cal_wet", val); }

uint8_t NVSStorage::getPumpMaxDuration() { return prefs.getUChar("pump_max", MAX_PUMP_DURATION_SEC); }
void NVSStorage::setPumpMaxDuration(uint8_t val) { prefs.putUChar("pump_max", val); }

uint8_t NVSStorage::getMoistureThreshold() { return prefs.getUChar("moist_thr", 30); }
void NVSStorage::setMoistureThreshold(uint8_t val) { prefs.putUChar("moist_thr", val); }

String NVSStorage::getWifiSSID() { return prefs.getString("wifi_ssid", "SmartVaseAP"); }
void NVSStorage::setWifiSSID(const String& ssid) { prefs.putString("wifi_ssid", ssid); }

String NVSStorage::getWifiPass() { return prefs.getString("wifi_pass", ""); }
void NVSStorage::setWifiPass(const String& pass) { prefs.putString("wifi_pass", pass); }

String NVSStorage::getAdminPass() { return prefs.getString("admin_pass", "admin"); }
void NVSStorage::setAdminPass(const String& pass) { prefs.putString("admin_pass", pass); }

bool NVSStorage::getSchedule(PumpSchedule& sched) {
    size_t len = prefs.getBytesLength("sched_0");
    if (len != sizeof(PumpSchedule)) return false;
    prefs.getBytes("sched_0", &sched, sizeof(PumpSchedule));
    return true;
}
void NVSStorage::setSchedule(const PumpSchedule& sched) {
    prefs.putBytes("sched_0", &sched, sizeof(PumpSchedule));
}
