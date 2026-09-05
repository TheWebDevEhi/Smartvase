#include "pump_controller.h"
#include "config.h"
#include "sensor_manager.h"
#include "power_manager.h"
#include "nvs_storage.h"

bool PumpController::pump_running = false;
uint32_t PumpController::pump_start_ms = 0;
uint32_t PumpController::pump_duration_ms = 0;
uint32_t PumpController::last_pump_time_unix = 0;

void PumpController::init() {
    digitalWrite(PIN_RELAY, RELAY_OFF); // Set state before enabling output
    pinMode(PIN_RELAY, OUTPUT);
    pump_running = false;
}

void PumpController::update() {
    if (pump_running) {
        // Check timeout
        if (millis() - pump_start_ms >= pump_duration_ms) {
            stopPump();
        }
        
        // Safety check during run
        if (PowerManager::getBatteryPercent() < BATTERY_CUTOFF_PCT) {
            stopPump();
        }
    }
}

bool PumpController::startPump(uint8_t duration_sec, bool override_safety) {
    if (pump_running) return false; // already running

    uint8_t max_duration = NVSStorage::getPumpMaxDuration();
    if (duration_sec > max_duration) duration_sec = max_duration;
    
    if (!override_safety) {
        // Cooldown check: prevent rapid successive pump triggers
        if (last_pump_time_unix > 0) {
            uint32_t elapsed_sec = (millis() / 1000) - last_pump_time_unix;
            if (elapsed_sec < PUMP_COOLDOWN_SEC) {
                Serial.println("Pump blocked: cooldown active.");
                return false;
            }
        }
    }

    pump_duration_ms = duration_sec * 1000;
    pump_start_ms = millis();
    pump_running = true;
    digitalWrite(PIN_RELAY, RELAY_ON); // Turn ON
    
    return true;
}

void PumpController::stopPump() {
    if (!pump_running) return;
    digitalWrite(PIN_RELAY, RELAY_OFF); // Turn OFF
    pump_running = false;
    last_pump_time_unix = millis() / 1000; // placeholder
}

bool PumpController::isPumpRunning() {
    return pump_running;
}

uint32_t PumpController::getLastPumpTime() {
    return last_pump_time_unix;
}
