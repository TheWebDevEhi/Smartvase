#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// --- PIN DEFINITIONS ---

// Sensors
#define PIN_DHT22 4
#define PIN_SOIL_MOISTURE 35
#define PIN_LDR 32
#define PIN_WATER_LEVEL 36
#define PIN_WATER_PWR 15
#define PIN_PIR 33
#define PIN_BATTERY 34

// Actuators
// Pump is wired to the relay's NO (Normally Open) contact, not NC.
// De-energized/unpowered relay -> contact open -> pump OFF (fail-safe).
// Energized relay (RELAY_ON driven) -> contact closed -> pump ON.
// Do NOT wire the pump back to NC: with no ESP/relay power at all, a
// de-energized relay defaults to its NC contact, which would leave the
// pump powered continuously any time the load supply is live.
#define PIN_RELAY 26
#define RELAY_ON HIGH // Configured for active-LOW (low-level trigger) relay
#define RELAY_OFF LOW // Configured for active-LOW (low-level trigger) relay

// Status LEDs
#define PIN_LED_SYSTEM 27  // Green - Healthy
#define PIN_LED_PUMP 12    // Blue - Pump active
#define PIN_LED_WIFI 13    // White - Wi-Fi connected/AP active
#define PIN_LED_BATTERY 14 // Yellow - Low battery (<20%)

// I2C (OLED)
#define PIN_I2C_SDA 21
#define PIN_I2C_SCL 22

// --- SYSTEM THRESHOLDS & CONSTANTS ---

// Pump safety
#define MAX_PUMP_DURATION_SEC 10
#define PUMP_COOLDOWN_SEC 30

// Water reservoir: below this graduated level (%), treat as "empty" for
// pump-safety gating (see NVSStorage::getWaterEmptyCal/getWaterFullCal for
// the raw ADC calibration points this percentage is derived from).
#define WATER_EMPTY_PCT_THRESHOLD 10

// Battery
#define BATTERY_MIN_PCT 20   // Below this, no auto-pump
#define BATTERY_CUTOFF_PCT 0 // Below this, force deep sleep
#define V_BAT_CAL_OFFSET 0.0 // Add actual offset measured by DMM here

// Sleep
#define DEFAULT_SLEEP_SEC 300           // 5 minutes
#define WEB_INACTIVITY_TIMEOUT_MS 60000 // 1 minute
#define AP_TIMEOUT_MS 300000            // 5 minutes

// NVS Namespace
#define NVS_NAMESPACE "smartvase"

// I2C Address
#define OLED_ADDR 0x3C

#endif // CONFIG_H
