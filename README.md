# ELEVATE-AI

**ELEVATE-AI** (Electronic Live Encouragement Via AI Text Enhancement) is an Arduino project designed to help improving office environments and encourage healthy work habits through real-time monitoring and AI-driven text prompts. Using sensors and AI analysis via the Gemini API, ELEVATE-AI provides a solution to combat office syndrome by reminding users to take breaks and optimize their workspace for productivity and well-being.

## Features

- Monitors environmental conditions in real-time: temperature, humidity, CO2 levels, UV index, and light intensity.
- Detects user presence at the desk using ultrasonic sensors.
- Leverages Node-RED for data processing and workflow automation.
- Utilizes the Gemini API for AI-driven insights and text prompts.
- Delivers a dashboard UI for data visualization and interaction.
- Sends automated, AI-powered reminders to encourage regular movement and breaks.

## Hardware Components

- 2 x ESP32-S3 modules
- DHT11 Temperature and Humidity Sensor
- LTR390 UV and Brightness Sensor
- MQ-135 Air Quality Sensor
- HC-SR04 Ultrasonic Sensor
- OLED Display I2C IIC 0.96″ SSD1306 128x64px

## Software Dependencies

- Arduino IDE
- Node-RED
- MQTT Broker

### Arduino Libraries

- [`WiFi`](https://github.com/arduino-libraries/WiFi) for ESP32's WiFi capabilities.
- [`PubSubClient`](https://github.com/knolleary/pubsubclient) for MQTT communications.
- [`Wire`](https://www.arduino.cc/reference/en/language/functions/communication/wire/) for I2C communications with sensors and display.
- [`Adafruit_Sensor`](https://github.com/adafruit/Adafruit_Sensor), `DHT` and `DHT_U` for the DHT sensors.
- [`LTR390`](https://github.com/levkovigor/LTR390) for the UV and ambient light sensor.
- [`MQ135`](https://github.com/NuclearPhoenixx/MQ135) for the air quality sensor.
- [`Adafruit_SSD1306`](https://github.com/adafruit/Adafruit_SSD1306) for the OLED display control.
- [`HCSR04`](https://github.com/gamegine/HCSR04-ultrasonic-sensor-lib) for the ultrasonic distance measurements.

## Configuration

### ESP32 Modules

- Update the `networkConfig.h` file in [`screen`](screen/networkConfig.h) and [`sensors`](/sensors/networkConfig.h) dictionary with your WiFi network details and MQTT broker information before flashing the ESP32 modules.

<details>

<summary>See <code>networkConfig.h</code></summary>

```c
#define WIFI_SSID    "<WIFI_SSID>"    // Config here
#define WIFI_PASS    "<WIFI_PASS>"    // Config here
#define MQTT_BROKER  "<MQTT_BROKER>"  // Config here
#define MQTT_USER    "<MQTT_USER>"    // Config here
#define MQTT_PASS    "<MQTT_PASS>"    // Config here


#define TOPIC_PREFIX "<TOPIC_PREFIX>" // Config here
#define TOPIC_DATA_PREFIX TOPIC_PREFIX "/sensors"

#define TOPIC_TEMPERATURE TOPIC_DATA_PREFIX "/temperature"
#define TOPIC_HUMIDITY TOPIC_DATA_PREFIX "/humidity"
#define TOPIC_AIR TOPIC_DATA_PREFIX "/air_quality"
#define TOPIC_UV TOPIC_DATA_PREFIX "/uv"
#define TOPIC_LIGHT TOPIC_DATA_PREFIX "/light"
#define TOPIC_JSON TOPIC_DATA_PREFIX "/json"

#define TOPIC_SITTING_PREFIX TOPIC_PREFIX "/sitting"
#define TOPIC_IS_SITTING TOPIC_SITTING_PREFIX "/is_sitting"
#define TOPIC_SITTING_DISTANCE TOPIC_SITTING_PREFIX "/distance"
#define TOPIC_SITTING_TIME TOPIC_SITTING_PREFIX "/time"
#define TOPIC_SITTING_JSON TOPIC_SITTING_PREFIX "/json"
#define TOPIC_SITTING_RESET TOPIC_SITTING_PREFIX "/reset"

#define TOPIC_TIME TOPIC_PREFIX "/time"


#define TOPIC_AI TOPIC_PREFIX "/ai/auto"

#define TOPIC_DEBUG TOPIC_PREFIX "/debug/data"
```

</details>

<br>

- Modify the `hardwareConfig.h` file in [`screen`](screen/hardwareConfig.h) and [`sensors`](sensors/hardwareConfig.h) dictionary to configure the ESP32 pin definitions to match your hardware setup before flashing the firmware onto the ESP32 modules.

<details>

<summary>See <code>hardwareConfig.h</code></summary>

<br>

```c
// screen/hardwareConfig.h

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define REFRESH_DELAY 16

#define OLED_SCL_PIN 47
#define OLED_SDA_PIN 48

#define US_ECHO_PIN 4
#define US_TRIGGER_PIN 5

#define LED_RED_PIN 42
#define LED_YELLOW_PIN 41
#define LED_GREEN_PIN 40
#define SWITCH_PIN 2
```

```c
// sensors/hardwareConfig.h

#define LTR390_I2C_ADDRESS 0x53
#define LTR_SDA_PIN 21
#define LTR_SCL_PIN 20

#define MQ135_DO_PIN 13
#define MQ135_AO_PIN 14

#define DHTPIN 15
#define DHTTYPE DHT11

#define LED_RED_PIN 42
#define LED_YELLOW_PIN 41
#define LED_GREEN_PIN 40
#define SWITCH_PIN 2
```

</details>

### Node-RED

- Set up the Node-RED environment variables to match your configuration, particularly the Gemini API keys and Discord chanel ID.

- Set up the Gemini API keys in the AI Dashboard Template

![Node-RED Environment](https://github.com/RJTPP/elevate-ai/assets/97868966/fb674c98-f0c1-46ac-9944-488450d92f25)

## AI Integration

Incorporates the **Gemini API** for AI-powered analysis and suggestions based on the collected environmental data.

## Installation

1. Flash the ESP32-S3 modules with the provided firmware, ensuring the `networkConfig.h` file is configured with your network details.
2. Set up Node-RED with the correct environment variables and configure the Gemini API integration.
3. Establish the MQTT broker for communication between the ESP32 modules and Node-RED.
4. Access the dashboard through Node-RED UI to monitor environmental conditions and receive AI-based recommendations.

---

**Develop by**

1. Rajata Thamcharoensatit [@RJTPP](https://github.com/RJTPP) - 6610502218
2. Phutthiphong Rodboung [@Rennis](https://github.com/PRennis) - 6610502170

Computer Engineering (CPE) Faculty, Kasetsart University, Bangkok, Thailand. 

This project was made for 01204114 Introduction to Computer Hardware Development.
