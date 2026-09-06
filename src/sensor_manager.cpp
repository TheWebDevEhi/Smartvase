#include "sensor_manager.h"
#include "config.h"
#include "nvs_storage.h"
#include <DHTesp.h>

static DHTesp dht;

void SensorManager::init() {
    pinMode(PIN_SOIL_MOISTURE, INPUT);
    pinMode(PIN_LDR, INPUT);
    
    // Water sensor pins
    pinMode(PIN_WATER_LEVEL, INPUT);
    pinMode(PIN_WATER_PWR, OUTPUT);
    digitalWrite(PIN_WATER_PWR, LOW); // Keep off by default
    
    // DHT22 init
    dht.setup(PIN_DHT22, DHTesp::DHT22);
}

void SensorManager::readAll(SensorData& data) {
    data.soilMoisturePct = readSoilMoisture();
    readDHT(data.temperature, data.humidity);
    data.lightLevel = readLight();
    readWaterLevel(data.waterLevelPct, data.waterPresent);
    data.timestamp = millis() / 1000; // placeholder timestamp
}

uint8_t SensorManager::readSoilMoisture() {
    uint32_t adc_sum = 0;
    for(int i=0; i<5; i++) {
        adc_sum += analogRead(PIN_SOIL_MOISTURE);
        delay(2);
    }
    uint16_t raw = adc_sum / 5;
    
    uint16_t dry_cal = NVSStorage::getCalDry();
    uint16_t wet_cal = NVSStorage::getCalWet();
    
    if (dry_cal == wet_cal) return 0; // prevent divide by zero
    
    // Convert to percentage (lower ADC = wetter for capacitive)
    // Map function equivalent:
    long pct = map(raw, dry_cal, wet_cal, 0, 100);
    if (pct < 0) pct = 0;
    if (pct > 100) pct = 100;
    
    return (uint8_t)pct;
}

void SensorManager::readDHT(float& temp, float& hum) {
    // Wait briefly if we just woke up (handled in main ideally, but we try here)
    TempAndHumidity tah = dht.getTempAndHumidity();
    if (dht.getStatus() == DHTesp::ERROR_NONE) {
        temp = tah.temperature;
        hum = tah.humidity;
    } else {
        temp = 0.0;
        hum = 0.0;
    }
}

uint16_t SensorManager::readLight() {
    // Invert the reading so that higher values = brighter light (ESP32 ADC is 12-bit, max 4095)
    return 4095 - analogRead(PIN_LDR);
}

void SensorManager::readWaterLevel(uint8_t& levelPct, bool& present) {
    // Power on sensor
    digitalWrite(PIN_WATER_PWR, HIGH);
    delay(10); // Wait for stabilization

    uint32_t adc_sum = 0;
    for(int i=0; i<3; i++) {
        adc_sum += analogRead(PIN_WATER_LEVEL);
        delay(1);
    }
    uint16_t raw = adc_sum / 3;

    // Power off sensor to prevent corrosion
    digitalWrite(PIN_WATER_PWR, LOW);

    uint16_t empty_cal = NVSStorage::getWaterEmptyCal();
    uint16_t full_cal = NVSStorage::getWaterFullCal();

    long pct = (empty_cal == full_cal) ? 0 : map(raw, empty_cal, full_cal, 0, 100);
    if (pct < 0) pct = 0;
    if (pct > 100) pct = 100;
    levelPct = (uint8_t)pct;
    present = levelPct > WATER_EMPTY_PCT_THRESHOLD;

    Serial.print("Water level raw ADC: ");
    Serial.print(raw);
    Serial.print(" -> ");
    Serial.print(levelPct);
    Serial.println("%");
}
