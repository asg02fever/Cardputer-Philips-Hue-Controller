/*
 * Firmware for M5Stack Cardputer controlling Philips Hue lights
 * Author: @keebasg
 * This firmware connects to a WiFi network and allows control of Philips Hue lights
 * using the M5Stack Cardputer. The firmware includes functionality to toggle lights,
 * adjust brightness, and set random colors.
 * Please replace the placeholders with your own WiFi credentials, Philips Hue Bridge IP,
 * API key, and light IDs.
 * If you make a fork of this code, please credit the original author (@keebasg).
 */

#include <M5Cardputer.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "KEEBASG.h"  // Include the bitmap header file

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// WiFi credentials - Replace with your own WiFi network SSID and password
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Philips Hue Bridge IP address and API key - Replace with your own bridge IP and API key
const char* hueBridgeIP = "YOUR_HUE_BRIDGE_IP"; // e.g., 192.168.1.100
const char* hueApiKey = "YOUR_HUE_API_KEY"; // e.g., 0CtM7QbMijCE0T5GimXjTDTjl9J1rcSqlvPxmShu

bool lightState = false; // State of the light
int brightness = 254; // Brightness level (1-254)

// Define an array of light IDs you want to control
int lightIDs[] = {1, 2, 3}; // Example light IDs, replace with your own light IDs add more after 2, 3, 4, 5, 6, 7};

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void showBootImage() {
    M5.Lcd.drawBitmap(0, 0, 240, 135, KEEBASG); // Adjust the parameters according to your image size
    delay(3000); // Show the boot image for 3 seconds
}

void displayMessage(const char* message, uint16_t color) {
    M5.Lcd.fillScreen(BLACK); // Clear the screen
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.println("Press key : ");
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(MAGENTA);
    M5.Lcd.setCursor(0, 25);
    M5.Lcd.println("Q=light ON/OFF");
    M5.Lcd.setCursor(0, 45);
    M5.Lcd.println("W=brightness 50%/100%");
    M5.Lcd.setCursor(0, 65);
    M5.Lcd.println("E=set random color");
    M5.Lcd.setTextColor(color);
    M5.Lcd.setCursor(0, 100);
    M5.Lcd.println(message);
    M5.Lcd.setTextColor(RED);
    M5.Lcd.setCursor(0, 120);
    M5.Lcd.println("made by @keebasg");
}

void controlLight(int lightIDs[], int numLights, const String& payload) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        for (int i = 0; i < numLights; i++) {
            String url = String("http://") + hueBridgeIP + "/api/" + hueApiKey + "/lights/" + String(lightIDs[i]) + "/state";
            http.begin(url);
            http.addHeader("Content-Type", "application/json");
            int httpResponseCode = http.PUT(payload);

            if (httpResponseCode > 0) {
                String response = http.getString();
                Serial.println(httpResponseCode);
                Serial.println(response);
            } else {
                Serial.print("Error on sending PUT for light ");
                Serial.print(lightIDs[i]);
                Serial.print(": ");
                Serial.println(httpResponseCode);
            }

            http.end();
        }
    } else {
        Serial.println("Error in WiFi connection");
    }
}

void toggleLights() {
    lightState = !lightState; // Toggle light state
    String payload = "{\"on\":" + String(lightState ? "true" : "false") + "}";
    controlLight(lightIDs, sizeof(lightIDs) / sizeof(lightIDs[0]), payload);
    displayMessage(lightState ? "Lights ON" : "Lights OFF", CYAN);
}

void toggleBrightness() {
    brightness = (brightness == 254) ? 127 : 254; // Toggle between 50% and 100%
    String payload = "{\"bri\":" + String(brightness) + "}";
    controlLight(lightIDs, sizeof(lightIDs) / sizeof(lightIDs[0]), payload);
    displayMessage((brightness == 254) ? "Brightness 100%" : "Brightness 50%", CYAN);
}

void setRandomColor() {
    int hue = random(0, 65535); // Random hue value
    String payload = "{\"hue\":" + String(hue) + "}";
    controlLight(lightIDs, sizeof(lightIDs) / sizeof(lightIDs[0]), payload);
    displayMessage("Random Color Set", CYAN);
}

void setup() {
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);
    Serial.begin(9600);

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    // Initialize the display
    M5.Lcd.begin();

    showBootImage();  // Show boot image

    // Example: Turn off lights initially
    controlLight(lightIDs, sizeof(lightIDs) / sizeof(lightIDs[0]), "{\"on\":false}");
    displayMessage("Initialized", CYAN);
}

void loop() {
    M5Cardputer.update();
    if (M5Cardputer.Keyboard.isChange()) {
        if (M5Cardputer.Keyboard.isKeyPressed('q')) {
            toggleLights();
        } else if (M5Cardputer.Keyboard.isKeyPressed('w')) {
            toggleBrightness();
        } else if (M5Cardputer.Keyboard.isKeyPressed('e')) {
            setRandomColor();
        }
    }
}
