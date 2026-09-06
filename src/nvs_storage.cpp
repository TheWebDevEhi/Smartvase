#include "nvs_storage.h"

Preferences NVSStorage::prefs;

void NVSStorage::init() {
    prefs.begin(NVS_NAMESPACE, false); // false = read/write mode
    // Initialize default values only if not already present (avoids unnecessary flash wear)
    if (!prefs.isKey("cal_dry"))   prefs.putUShort("cal_dry", 4095);
    if (!prefs.isKey("cal_wet"))   prefs.putUShort("cal_wet", 1500);
    // Placeholder water-level calibration - tune from the "Water level raw ADC" serial
    // log (empty reservoir vs. fully submerged probe) once the pull-down resistor is in place.
    if (!prefs.isKey("wlvl_empty")) prefs.putUShort("wlvl_empty", 0);
    if (!prefs.isKey("wlvl_full"))  prefs.putUShort("wlvl_full", 3000);
    if (!prefs.isKey("pump_max"))  prefs.putUChar("pump_max", MAX_PUMP_DURATION_SEC);
    if (!prefs.isKey("moist_thr")) prefs.putUChar("moist_thr", 30);
    if (!prefs.isKey("admin_pass")) prefs.putString("admin_pass", "admin");
}

uint16_t NVSStorage::getCalDry() { return prefs.getUShort("cal_dry", 4095); }
void NVSStorage::setCalDry(uint16_t val) { prefs.putUShort("cal_dry", val); }

uint16_t NVSStorage::getCalWet() { return prefs.getUShort("cal_wet", 1500); }
void NVSStorage::setCalWet(uint16_t val) { prefs.putUShort("cal_wet", val); }

uint16_t NVSStorage::getWaterEmptyCal() { return prefs.getUShort("wlvl_empty", 0); }
void NVSStorage::setWaterEmptyCal(uint16_t val) { prefs.putUShort("wlvl_empty", val); }

uint16_t NVSStorage::getWaterFullCal() { return prefs.getUShort("wlvl_full", 3000); }
void NVSStorage::setWaterFullCal(uint16_t val) { prefs.putUShort("wlvl_full", val); }

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
