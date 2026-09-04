#include "display_manager.h"
#include "config.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool DisplayManager::is_on = false;
uint32_t DisplayManager::last_activity_ms = 0;

void DisplayManager::init() {
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println("SSD1306 allocation failed");
        return;
    }
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0,0);
    display.println("Smart Vase Init");
    display.display();
    
    is_on = true;
    last_activity_ms = millis();
}

void DisplayManager::update() {
    if (!is_on) return;
    
    // Auto dim/off logic
    if (millis() - last_activity_ms > 10000) {
        // Dim display (if supported, else just wait for full off)
        display.dim(true);
    }
    
    // Throttle display updates to avoid jamming the I2C bus
    static uint32_t last_draw_ms = 0;
    if (millis() - last_draw_ms < 500) return;
    last_draw_ms = millis();
    
    // Draw basic UI
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Smart Vase");
    display.println("-----------");
    
    if (WiFi.status() == WL_CONNECTED) {
        display.println("WiFi: Connected");
        display.print("IP: ");
        display.println(WiFi.localIP());
    } else if (WiFi.getMode() == WIFI_AP_STA || WiFi.getMode() == WIFI_AP) {
        display.println("AP: SmartVase_Setup");
        display.print("IP: ");
        display.println(WiFi.softAPIP());
    } else {
        display.println("WiFi: Offline");
    }
    
    display.display();
}

void DisplayManager::turnOn() {
    if (!is_on) {
        display.ssd1306_command(SSD1306_DISPLAYON);
        is_on = true;
    }
    last_activity_ms = millis();
    display.dim(false);
}

void DisplayManager::turnOff() {
    if (is_on) {
        display.ssd1306_command(SSD1306_DISPLAYOFF);
        is_on = false;
    }
}
