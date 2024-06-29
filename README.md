# M5Stack Cardputer Philips Hue Controller

This project provides firmware for the M5Stack Cardputer to control Philips Hue lights. The firmware connects to a WiFi network and allows control of Philips Hue lights using the M5Stack Cardputer. The functionalities include toggling lights on and off, adjusting brightness, and setting random colors.

## Features

- Connects to a specified WiFi network.
- Controls Philips Hue lights using the M5Stack Cardputer.
- Toggle lights on/off with the 'Q' key.
- Adjust brightness between 50% and 100% with the 'W' key.
- Set a random color with the 'E' key.

## Requirements

- M5Stack Cardputer
- Philips Hue Bridge
- Arduino IDE with necessary libraries:
  - M5Cardputer
  - WiFi
  - HTTPClient
  - ArduinoJson

## Setup

1. Clone this repository:
   ```bash
   git clone https://github.com/yourusername/M5Stack-Cardputer-Philips-Hue-Controller.git
   cd M5Stack-Cardputer-Philips-Hue-Controller
2. rename the folder "M5Stack-Cardputer-Philips-Hue-Controller-main" to "hue_cardputter"

3. Open hue_cardputter.ino file in Arduino IDE.

4. Replace the placeholders with your own information:
   ```bash    
   WiFi SSID and password:
   const char* ssid = "YOUR_SSID";
   const char* password = "YOUR_PASSWORD";

   Philips Hue Bridge IP address and API key:
   const char* hueBridgeIP = "YOUR_HUE_BRIDGE_IP";
   const char* hueApiKey = "YOUR_HUE_API_KEY";
   
   Light IDs:
   int lightIDs[] = {1, 2, 3};
   ```
   you can get your api [there.](https://developers.meethue.com/develop/get-started-2/)
   by following the official instructions

6. Upload the firmware to your M5Stack Cardputer.

## Usage 

1. Power on the M5Stack Cardputer.
2. The Cardputer will attempt to connect to the specified WiFi network.
3. Once connected, you can control the lights using the following keys: 
  - Q: Toggle lights on/off
  - W: Adjust brightness between 50% and 100%
  - E: Set a random color

## Attribution
If you make a fork of this code, please credit the original author @keebasg and contributor @Elit3snip3r. You can find @Elit3snip3r's GitHub profile [here](https://github.com/Elitesniper06).


