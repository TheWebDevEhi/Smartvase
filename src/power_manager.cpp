#include "power_manager.h"
#include "config.h"
#include "display_manager.h"

WakeCause PowerManager::currentWakeCause = WakeCause::UNKNOWN;

void PowerManager::init() {
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    
    switch(wakeup_reason) {
        case ESP_SLEEP_WAKEUP_EXT0:     currentWakeCause = WakeCause::PIR_MOTION; break;
        case ESP_SLEEP_WAKEUP_TIMER:    currentWakeCause = WakeCause::TIMER_WAKE; break;
        case ESP_SLEEP_WAKEUP_UNDEFINED: currentWakeCause = WakeCause::POWER_ON; break;
        default:                        currentWakeCause = WakeCause::UNKNOWN; break;
    }

    // Configure battery monitor pin
    pinMode(PIN_BATTERY, INPUT);
    analogSetPinAttenuation(PIN_BATTERY, ADC_11db); // Full range 0-3.3V for battery pin only
}

WakeCause PowerManager::getWakeCause() {
    return currentWakeCause;
}

float PowerManager::readBatteryVoltageRaw() {
    // Read ADC and average over 10 samples
    uint32_t adc_sum = 0;
    for (int i = 0; i < 10; i++) {
        adc_sum += analogRead(PIN_BATTERY);
        delay(2);
    }
    float avg_adc = adc_sum / 10.0;
    
    // Convert to voltage (Voltage divider: 100k/100k -> 0.5 ratio)
    // 3.3V reference, 12-bit ADC (4095)
    // V_bat = (avg_adc / 4095.0) * 3.3 * 2.0
    float v_pin = (avg_adc / 4095.0) * 3.3;
    float v_bat = (v_pin * 2.0) + V_BAT_CAL_OFFSET;
    
    return v_bat;
}

float PowerManager::getBatteryVoltage() {
    return readBatteryVoltageRaw();
}

uint8_t PowerManager::getBatteryPercent() {
    float v_bat = readBatteryVoltageRaw();
    
    // Simple linear approximation of 18650 discharge curve
    if (v_bat >= 4.20) return 100;
    if (v_bat <= 3.00) return 0;
    
    // Estimate based on piecewise
    if (v_bat > 3.9) return 75 + (v_bat - 3.9) * (25.0 / 0.3);
    if (v_bat > 3.7) return 50 + (v_bat - 3.7) * (25.0 / 0.2);
    if (v_bat > 3.5) return 25 + (v_bat - 3.5) * (25.0 / 0.2);
    if (v_bat > 3.3) return 10 + (v_bat - 3.3) * (15.0 / 0.2);
    return (v_bat - 3.0) * (10.0 / 0.3);
}

void PowerManager::goToSleep(uint32_t sleep_seconds) {
    // Configure wake up sources
    esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_PIR, 1); // 1 = High
    esp_sleep_enable_timer_wakeup((uint64_t)sleep_seconds * 1000000ULL);
    
    Serial.println("Going to deep sleep now...");
    DisplayManager::turnOff();
    delay(100);
    esp_deep_sleep_start();
}
