#include "led_indicator.h"
#include "config.h"
#include "pump_controller.h"
#include "power_manager.h"

bool LedIndicator::sys_healthy = true;
bool LedIndicator::wifi_active = false;
uint32_t LedIndicator::blink_ms = 0;

void LedIndicator::init() {
    pinMode(PIN_LED_SYSTEM, OUTPUT);
    pinMode(PIN_LED_PUMP, OUTPUT);
    pinMode(PIN_LED_WIFI, OUTPUT);
    pinMode(PIN_LED_BATTERY, OUTPUT);
    
    digitalWrite(PIN_LED_SYSTEM, LOW);
    digitalWrite(PIN_LED_PUMP, LOW);
    digitalWrite(PIN_LED_WIFI, LOW);
    digitalWrite(PIN_LED_BATTERY, LOW);
}

void LedIndicator::update() {
    // Basic status updates
    digitalWrite(PIN_LED_SYSTEM, sys_healthy ? HIGH : LOW);
    digitalWrite(PIN_LED_PUMP, PumpController::isPumpRunning() ? HIGH : LOW);
    digitalWrite(PIN_LED_WIFI, wifi_active ? HIGH : LOW);
    
    // Battery warning
    if (PowerManager::getBatteryPercent() <= BATTERY_MIN_PCT) {
        // Blink yellow LED
        if (millis() - blink_ms > 500) {
            blink_ms = millis();
            digitalWrite(PIN_LED_BATTERY, !digitalRead(PIN_LED_BATTERY));
        }
    } else {
        digitalWrite(PIN_LED_BATTERY, LOW);
    }
}

void LedIndicator::setSystemHealthy(bool healthy) {
    sys_healthy = healthy;
}

void LedIndicator::setWifiState(bool active) {
    wifi_active = active;
}
