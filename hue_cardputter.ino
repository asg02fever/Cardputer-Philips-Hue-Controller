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
const char* hueApiKey = "YOUR_HUE_API_KEY"; // e.g., 

bool lightState = false; // State of the light
int brightness = 254; // Brightness level (1-254)

// Define an array of light IDs you want to control
int lightIDs[] = {1, 2, 3}; // Example light IDs, replace with your own light IDs add more after 2, 3, 4, 5, 6, 7};

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

enum Mode {
    Normal,
    SelectingColor
};

Mode currentMode = Normal;

// Define color options with CIE xy coordinates
const int NUM_COLORS = 13;
const char* colorNames[NUM_COLORS] = {"Red", "Green", "Blue", "Yellow", "Purple", "Cyan", "White", "Orange", "Pink", "Lime", "Dodo", "Custom 1", ""};
float xyColors[NUM_COLORS][2] = {
    {0.675, 0.322},   // Red
    {0.408, 0.517},   // Green
    {0.167, 0.04},    // Blue
    {0.444, 0.516},   // Yellow
    {0.322, 0.147},   // Purple
    {0.15, 0.3},      // Cyan
    {0.3227, 0.329},  // White
    {0.5614, 0.4152}, // Orange
    {0.4153, 0.192},  // Pink
    {0.409, 0.518},   // Lime
    {0.519, 0.418},   // Dodo
    {0.179, 0.322},   // Custom 1
    {0.0, 0.0}        // Empty slot for scrolling
};
int selectedColor = 0;

void showBootImage() {
    M5.Lcd.drawBitmap(0, 0, 240, 135, KEEBASG); // Adjust the parameters according to your image size
    delay(3000); // Show the boot image for 3 seconds
}

void displayMessage(const char* message, uint16_t color) {
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(TFT_CYAN);
    M5.Lcd.setCursor(0, 20);
    M5.Lcd.println("Light Control");

    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.setCursor(0, 40);
    M5.Lcd.println("Q: Toggle Light");

    M5.Lcd.setCursor(0, 60);
    M5.Lcd.println("W: Adjust Brightness");

    M5.Lcd.setCursor(0, 80);
    M5.Lcd.println("E: Change Color");

    M5.Lcd.setCursor(0, 100);
    M5.Lcd.println("R: About");

    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(color);
    M5.Lcd.setCursor(0, 120);
    M5.Lcd.println(message);
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
    
    // Affichage du message avec la couleur appropriée
    if (lightState) {
        displayMessage("Lights ON", TFT_GREEN);
    } else {
        displayMessage("Lights OFF", TFT_RED);
    }
}

void toggleBrightness() {
    brightness = (brightness == 254) ? 127 : 254; // Toggle between 50% and 100%
    String payload = "{\"bri\":" + String(brightness) + "}";
    controlLight(lightIDs, sizeof(lightIDs) / sizeof(lightIDs[0]), payload);
    displayMessage((brightness == 254) ? "Brightness 100%" : "Brightness 50%", TFT_GREEN);
}

void displayColorMenu(int selectedColor) {
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(TFT_CYAN);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.println("Select Color:");
    M5.Lcd.setTextColor(TFT_GREEN);
    M5.Lcd.setCursor(0, 15);
    M5.Lcd.println("press OK to confirm");

    // Calculate maximum visible colors on the screen
    int maxVisibleColors = M5.Lcd.height() / 20; // Assuming each color option is 20 pixels tall

    // Calculate starting index based on selected color and max visible colors
    int startIdx = selectedColor - maxVisibleColors / 2;
    if (startIdx < 0) {
        startIdx = 0;
    } else if (startIdx > NUM_COLORS - maxVisibleColors) {
        startIdx = NUM_COLORS - maxVisibleColors;
    }

    for (int i = startIdx; i < startIdx + maxVisibleColors; i++) {
        if (i >= 0 && i < NUM_COLORS) {
            if (i == selectedColor) {
                M5.Lcd.setTextSize(2);
                M5.Lcd.setTextColor(TFT_GREEN);
            } else {
                M5.Lcd.setTextSize(2);
                M5.Lcd.setTextColor(TFT_WHITE);
            }
            M5.Lcd.setCursor(0, 35 + (i - startIdx) * 20);
            M5.Lcd.println(colorNames[i]);
        }
    }
}

void selectColor() {
    displayColorMenu(selectedColor);

    while (true) {
        M5Cardputer.update();

        if (M5Cardputer.Keyboard.isChange()) {
            if (M5Cardputer.Keyboard.isKeyPressed('.')) {
                selectedColor = (selectedColor + 1) % NUM_COLORS;
                displayColorMenu(selectedColor);
            } else if (M5Cardputer.Keyboard.isKeyPressed(';')) {
                selectedColor = (selectedColor - 1 + NUM_COLORS) % NUM_COLORS;
                displayColorMenu(selectedColor);
            } else if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER)) {
                float x = xyColors[selectedColor][0];
                float y = xyColors[selectedColor][1];
                int brightnessValue = 254; // Set brightness as desired
                String payload = "{\"xy\":[" + String(x, 4) + "," + String(y, 4) + "],\"bri\":" + String(brightnessValue) + "}";
                controlLight(lightIDs, sizeof(lightIDs) / sizeof(lightIDs[0]), payload);
              //  displayMessage(("Color Set: " + String(colorNames[selectedColor])).c_str(), TFT_YELLOW);
                selectedColor = 0; // Reset to default color (Red)
            } else if (M5Cardputer.Keyboard.isKeyPressed('`')) {
                displayMessage("Color Selection Cancelled", TFT_RED);
                break; // Exit color selection mode
            }
        }
    }
}


void drawStatusBar() {
    M5.Lcd.fillRect(0, 0, 320, 20, TFT_DARKGREY);
    M5.Lcd.setTextColor(TFT_GREEN);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(5, 5);
    M5.Lcd.print(WiFi.status() == WL_CONNECTED ? "WiFi: Connected" : "WiFi: Disconnected");
}

void showAboutPage() {
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(TFT_CYAN);
    M5.Lcd.setCursor(0, 10);
    M5.Lcd.println("About");

    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.setCursor(0, 35);
    M5.Lcd.println("This is a controler");
    M5.Lcd.println("controler for");
    M5.Lcd.println("Philips Hue lights.");

    M5.Lcd.setCursor(0, 100);
    M5.Lcd.println("Made by "); //@keebasg.
    M5.Lcd.setTextColor(TFT_RED);
    M5.Lcd.setCursor(90, 100);
    M5.Lcd.println("@keebasg. ");
    
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(TFT_GREEN);
    M5.Lcd.setCursor(0, 120);
    M5.Lcd.println("Version: 2.2.1");
    
    while (true) {
        M5Cardputer.update();

        if (M5Cardputer.Keyboard.isChange()) {
             if (M5Cardputer.Keyboard.isKeyPressed('`')) {
                displayMessage("Returning to main screen", TFT_YELLOW);
                break; // Exit color selection mode
            }
        }
    }
   //delay(5000); // Display the about page for 5 seconds
   //displayMessage("Returning to main screen", TFT_YELLOW);
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
    displayMessage("Initialized", TFT_YELLOW);
}

void loop() {
    M5Cardputer.update();

    if (M5Cardputer.Keyboard.isChange()) {
        if (M5Cardputer.Keyboard.isKeyPressed('q')) {
            if (currentMode == Normal) {
                toggleLights();
            }
        } else if (M5Cardputer.Keyboard.isKeyPressed('w')) {
            if (currentMode == Normal) {
                toggleBrightness();
            }
        } else if (M5Cardputer.Keyboard.isKeyPressed('e')) {
            if (currentMode == Normal) {
                currentMode = SelectingColor;
                selectColor();
                currentMode = Normal;
            }
        } else if (M5Cardputer.Keyboard.isKeyPressed('r')) {
            if (currentMode == Normal) {
                showAboutPage();
            }
        }
    }

    // Update battery status every minute
    static unsigned long lastUpdateTime = 0;
    if (millis() - lastUpdateTime > 500) {
        drawStatusBar();
        lastUpdateTime = millis();
    }
}
