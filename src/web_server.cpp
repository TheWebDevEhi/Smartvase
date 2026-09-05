#include "web_server.h"
#include "config.h"
#include "nvs_storage.h"
#include "sensor_manager.h"
#include "pump_controller.h"
#include "power_manager.h"
#include "led_indicator.h"
#include "display_manager.h"
#include "webassets.h"

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

static AsyncWebServer server(80);

uint32_t WebServerManager::boot_time = 0;
bool WebServerManager::ap_active = false;
uint32_t WebServerManager::last_api_call_ms = 0;
bool WebServerManager::should_reboot = false;
uint32_t WebServerManager::reboot_time = 0;

void WebServerManager::init() {
    boot_time = millis();
    last_api_call_ms = millis();
    
    // Connect to STA
    String ssid = NVSStorage::getWifiSSID();
    String pass = NVSStorage::getWifiPass();
    
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("SmartVase_Setup", "password123"); // Default AP credentials
    ap_active = true;
    
    Serial.println("AP Mode started. Connect to 'SmartVase_Setup' with password 'password123'");
    Serial.print("AP IP Address: ");
    Serial.println(WiFi.softAPIP());
    
    if (ssid.length() > 0) {
        WiFi.begin(ssid.c_str(), pass.c_str());
    }

    setupEndpoints();
    server.begin();
    
    LedIndicator::setWifiState(true);
}

void WebServerManager::update() {
    if (ap_active && (millis() - boot_time > AP_TIMEOUT_MS)) {
        // Turn off AP mode after 90 seconds
        WiFi.softAPdisconnect(true);
        WiFi.mode(WIFI_STA);
        ap_active = false;
        Serial.println("AP Mode disabled.");
    }
    
    // Check if we should sleep due to web inactivity
    // Only if we were woken up for web access (or if we are just idling)
    uint32_t current_timeout = ap_active ? AP_TIMEOUT_MS : WEB_INACTIVITY_TIMEOUT_MS;
    if (millis() - last_api_call_ms > current_timeout) {
        // Sleep if pump is not running
        if (!PumpController::isPumpRunning()) {
            Serial.println("Sleeping due to web inactivity.");
            PowerManager::goToSleep(DEFAULT_SLEEP_SEC);
        }
    }
    
    // Handle async reboot safely
    if (should_reboot && millis() - reboot_time > 1000) {
        Serial.println("Rebooting device now...");
        ESP.restart();
    }
}

void WebServerManager::handleClientActive() {
    last_api_call_ms = millis();
}

void WebServerManager::setupEndpoints() {
    // Serve HTML
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        handleClientActive();
        request->send(200, "text/html", index_html);
    });

    // API Sensors
    server.on("/api/sensors", HTTP_GET, [](AsyncWebServerRequest *request){
        handleClientActive();
        SensorData data;
        SensorManager::readAll(data);
        
        JsonDocument doc;
        doc["ts"] = data.timestamp;
        doc["soil"] = data.soilMoisturePct;
        doc["temp"] = data.temperature;
        doc["hum"] = data.humidity;
        doc["light"] = data.lightLevel;
        doc["water"] = data.waterPresent ? 1 : 0;
        doc["bat"] = PowerManager::getBatteryPercent();
        
        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });

    // Wake the OLED from its dimmed/idle state (dashboard "wake screen" button)
    server.on("/api/display/wake", HTTP_POST, [](AsyncWebServerRequest *request){
        handleClientActive();
        DisplayManager::turnOn();
        request->send(200, "text/plain", "Display woken");
    });

    // Handle POST data (basic implementation for ArduinoJson 7)
    server.on("/api/pump", HTTP_POST, [](AsyncWebServerRequest *request){}, NULL,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
            handleClientActive();
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, data, len);
            if (error) {
                request->send(400, "text/plain", "Invalid JSON");
                return;
            }
            
            String action = doc["action"] | "";
            if (action == "start") {
                uint8_t dur = doc["duration"] | 5;
                if (PumpController::startPump(dur, false)) { // cooldown enforced for manual triggers too
                    request->send(200, "text/plain", "Pump started");
                } else {
                    request->send(429, "text/plain", "Pump blocked (already running or cooling down)");
                }
            } else if (action == "stop") {
                PumpController::stopPump();
                request->send(200, "text/plain", "Pump stopped");
            } else {
                request->send(400, "text/plain", "Unknown action");
            }
    });
    
    // Config and WiFi endpoints can be implemented similarly
    server.on("/api/wifi", HTTP_POST, [](AsyncWebServerRequest *request){}, NULL,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
            handleClientActive();
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, data, len);
            if (error) {
                request->send(400, "text/plain", "Invalid JSON");
                return;
            }
            
            String admin_pass = doc["admin_pass"] | "";
            if (admin_pass != NVSStorage::getAdminPass()) {
                request->send(401, "text/plain", "Unauthorized");
                return;
            }
            
            String ssid = doc["ssid"] | "";
            String pass = doc["password"] | "";
            
            if (ssid.length() > 0) {
                NVSStorage::setWifiSSID(ssid);
                NVSStorage::setWifiPass(pass);
                request->send(200, "text/plain", "Saved");
                
                // Trigger safe reboot
                should_reboot = true;
                reboot_time = millis();
            } else {
                request->send(400, "text/plain", "Invalid SSID");
            }
    });
}
