#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>

struct SensorData {
    uint8_t soilMoisturePct;
    float temperature;
    float humidity;
    uint16_t lightLevel;
    bool waterPresent;
    uint32_t timestamp;
};

class SensorManager {
public:
    static void init();
    static void readAll(SensorData& data);

private:
    static uint8_t readSoilMoisture();
    static void readDHT(float& temp, float& hum);
    static uint16_t readLight();
    static bool readWaterLevel();
};

#endif // SENSOR_MANAGER_H
