/*
 * Copyright (c) 2024 RJTPP
 *
 * This software is licensed under the MIT License. Please see the LICENSE file for details.
 *
 * SPDX-License-Identifier: MIT
 *
 * For more information and contributions, please visit:
 * https://github.com/RJTPP
 */


#include "esp32-hal.h"
#include <sys/_stdint.h>


// ** Variable **

LTR390 ltr390(LTR390_I2C_ADDRESS);
DHT dht(DHTPIN, DHTTYPE);
MQ135 gasSensor = MQ135(MQ135_AO_PIN);
WiFiClient wifiClient;
PubSubClient mqtt(MQTT_BROKER, 1883, wifiClient);

// TIme
uint32_t currentInternalTime = millis();

// Connection
bool connectInProgress = false;
uint32_t lastMqttAttempt = millis();

// Switch
int switchStatus = 1;
bool isToggling = false;
bool isLongPress = false;
bool isOn = true;

// Data
float airQuality = 0.0;
float humidity = 0.0;
float temperature = 0.0;
int lightLevel = 0;
int uv = 0;
int uvRaw = 0;

// Delay
uint32_t lastDelayLoop = millis();

// ** Function declaration **

// MQTT
void connect_wifi();
void reconnect_wifi();
void connect_mqtt();
void reconnectMQTT();
void mqtt_callback(char* topic, byte* payload, unsigned int length);

// Switch
void updateToggleStatus();
bool checkToggling();
void updateIsOnStatus();
void updateStatusLight();

// Data
float* dhtSensorRead();
void updateLtr390Data();
void updateAlldata();
void uploadAllData();
void uploadAllZeroData();

// Debug
void debugprintData();
void uploadDebug();