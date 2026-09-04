#include <Arduino.h>
#include "config.h"
#include "power_manager.h"
#include "sensor_manager.h"
#include "pump_controller.h"
#include "display_manager.h"
#include "led_indicator.h"
#include "web_server.h"
#include "nvs_storage.h"

void setup() {
    Serial.begin(115200);
    delay(100);
    Serial.println("Smart Vase Starting...");

    // Initialize all subsystems
    NVSStorage::init();
    PowerManager::init();
    LedIndicator::init();
    DisplayManager::init();  // Init display (and I2C) early, before sensor reads
    PumpController::init();
    SensorManager::init();
    
    // Check wake cause
    WakeCause cause = PowerManager::getWakeCause();

    // Always read sensors on wake
    SensorData currentData;
    SensorManager::readAll(currentData);

    // Auto-Pump logic
    if (cause == WakeCause::TIMER_WAKE || cause == WakeCause::POWER_ON) {
        uint8_t thr = NVSStorage::getMoistureThreshold();
        if (currentData.soilMoisturePct < thr && currentData.waterPresent) {
            if (PowerManager::getBatteryPercent() > BATTERY_MIN_PCT) {
                // start pump for default duration, without overriding safety
                PumpController::startPump(NVSStorage::getPumpMaxDuration(), false); 
                Serial.println("Auto-pump triggered based on sensor data.");
            } else {
                Serial.println("Auto-pump blocked: Battery too low.");
            }
        }
    }
    
    if (cause == WakeCause::PIR_MOTION) {
        Serial.println("Woke up from PIR.");
    } else if (cause == WakeCause::TIMER_WAKE) {
        Serial.println("Woke up from Timer.");
        // Optional: Turn off the display on timer wake to save power:
        // DisplayManager::turnOff();
    } else {
        Serial.println("Power ON reset.");
    }

    WebServerManager::init();
}

void loop() {
    // Non-blocking updates
    PumpController::update();
    DisplayManager::update();
    LedIndicator::update();
    WebServerManager::update();
    
    delay(10); // Small delay to yield to FreeRTOS tasks (e.g. AsyncWebServer)
}
