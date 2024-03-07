/*
 * Copyright (c) 2024 RJTPP
 *
 * This software is licensed under the MIT License. Please see the LICENSE file for details.
 *
 * SPDX-License-Identifier: MIT
 *
 * For more information and contributions, please visit:
 * https://github.com/RJTPP/ELEVATE-AI
 */


// Network
#include <WiFi.h>
#include <PubSubClient.h>

// LTR 390
#include <Wire.h>
#include <LTR390.h>

// DHT
#include <Adafruit_Sensor.h>
#include <DHT.h>  // DHT sensor library
#include <DHT_U.h>

// Local
#include "MQ135.h"  // MQ135
#include "hardwareConfig.h"
#include "networkConfig.h"
#include "sensors.h"


// ** Setup **
void setup() {
  Serial.begin(115200);

  // DHT
  pinMode(DHTPIN, INPUT_PULLUP);

  // MQ135
  pinMode(MQ135_AO_PIN, INPUT_PULLUP);
  pinMode(MQ135_DO_PIN, INPUT_PULLUP);

  // Switch & LED
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_YELLOW_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);

  digitalWrite(LED_YELLOW_PIN, HIGH);

  // LTR390
  Wire.begin(LTR_SDA_PIN, LTR_SCL_PIN);
  // Initialize the LTR390 sensor. If it fails, print an error message.
  if (!ltr390.init()) {
    Serial.println("LTR390 not connected!");
  }

  // Connect to network
  connect_wifi();
  connect_mqtt();

  // Ready -> green light
  delay(1000);
  digitalWrite(LED_GREEN_PIN, HIGH);
  digitalWrite(LED_YELLOW_PIN, LOW);
}


// ** Loop **
void loop() {
  // Serial.println("[APP] Free memory: " + String(esp_get_free_heap_size()) + " bytes\n");
  currentInternalTime = millis();

  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_RED_PIN, HIGH);
    reconnect_wifi();
  }

  if (!mqtt.connected()) {
    digitalWrite(LED_RED_PIN, HIGH);
    if (currentInternalTime - lastMqttAttempt >= 5000) {
      Serial.println("Reconnecting MQTT");
      reconnectMQTT();  // Implement this function to handle reconnection
      lastMqttAttempt = millis();
    }
  } else {
    digitalWrite(LED_RED_PIN, LOW);
  }

  mqtt.loop();

  // On off switch
  switchStatus = digitalRead(SWITCH_PIN);
  updateIsOnStatus();

  // on-off light
  updateStatusLight();

  // Sensors delay
  if (millis() - lastDelayLoop >= 2000) {
    if (isOn) {
      // Get data from sensors & update to variable
      updateAlldata();

      // debugprintData();
      uploadAllData();

      lastDelayLoop = millis();
    } else {
      uploadAllZeroData();
    }

    uploadDebug();
  }
  
  // if (!isOn) {
  //   delay(800);
  // }
  delay(200);
}


// ** Function **

// * MQTT *
void connect_wifi() {
  printf("WiFi MAC address is %s\n", WiFi.macAddress().c_str());
  printf("Connecting to WiFi %s.\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    printf(".");
    fflush(stdout);
    delay(500);
  }
  printf("\nWiFi connected.\n");
}

void reconnect_wifi() {
  static unsigned long lastAttemptTime = 0;
  const unsigned long retryInterval = 5000;  // Retry every 5 seconds

  if (!connectInProgress) {
    if (currentInternalTime - lastAttemptTime > retryInterval) {
      Serial.println("Wifi connection lost. Reconnecting Wifi");
      printf("WiFi MAC address is %s\n", WiFi.macAddress().c_str());
      printf("Reconnecting to WiFi %s.\n", WIFI_SSID);
      WiFi.disconnect();
      WiFi.mode(WIFI_STA);
      WiFi.begin(WIFI_SSID, WIFI_PASS);
      connectInProgress = true;
      lastAttemptTime = currentInternalTime;  // Update the last attempt time
    }
  } else {
    if (WiFi.status() == WL_CONNECTED) {
      printf("\nWiFi reconnected. IP address: %s\n", WiFi.localIP().toString().c_str());
      connectInProgress = false;  // Reset flag on successful connection
    } else {
      printf(".");
      fflush(stdout);
    }
  }
}

void connect_mqtt() {
  printf("Connecting to MQTT broker at %s.\n", MQTT_BROKER);
  if (!mqtt.connect("", MQTT_USER, MQTT_PASS)) {
    printf("Failed to connect to MQTT broker.\n");
    for (;;) {}  // wait here forever
  }
  mqtt.setCallback(mqtt_callback);
  printf("MQTT broker connected.\n");
}

void reconnectMQTT() {
  printf("Reconnecting to MQTT broker at %s.\n", MQTT_BROKER);
  if (mqtt.connect("", MQTT_USER, MQTT_PASS)) {
    mqtt.setCallback(mqtt_callback);
    // mqtt.subscribe(TOPIC_DEBUG);

    printf("MQTT broker reconnected.\n");
  } else {
    printf("Failed to reconnect to MQTT broker.\n");
  }
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  // null-terminate the payload to treat it as a string
  payload[length] = 0;

  // Debug
  printf("Callback: {topic: %s, payload: %s}\n", topic, payload);
}

// * Status *
void updateToggleStatus() {
  if (digitalRead(SWITCH_PIN) == 0) {
    if (!isLongPress) {
      isLongPress = true;
      isToggling = !isToggling;
    }
  } else {
    isLongPress = false;
  }
  return;
}

bool checkToggling() {
  updateToggleStatus();
  if (isToggling) {
    isToggling = false;
    return true;
  }
  return false;
}

void updateIsOnStatus() {
  if (checkToggling()) {
    isOn = !isOn;
  }
}

void updateStatusLight() {
  if (isOn) {
    // On
    digitalWrite(LED_GREEN_PIN, HIGH);
    digitalWrite(LED_YELLOW_PIN, LOW);
  } else {
    // Standby mode
    digitalWrite(LED_GREEN_PIN, LOW);
    digitalWrite(LED_YELLOW_PIN, HIGH);
  }
}

// * Read data from sensor *
float* dhtSensorRead() {
  static float data[2];
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    data[0] = -1;
    data[1] = -1;
    return data;
  }

  data[0] = humidity;
  data[1] = temperature;

  return data;
}

// * Update data *
void updateLtr390Data() {
  if (ltr390.newDataAvailable()) {
    // If the sensor is in ALS mode, read the ambient light in lux.
    if (ltr390.getMode() == LTR390_MODE_ALS) {
      lightLevel = ltr390.getLux();

      ltr390.setGain(LTR390_GAIN_18);
      ltr390.setResolution(LTR390_RESOLUTION_20BIT);
      ltr390.setMode(LTR390_MODE_UVS);
    } else if (ltr390.getMode() == LTR390_MODE_UVS) {

      uv = ltr390.getUVI();
      uvRaw = ltr390.readUVS();

      ltr390.setGain(LTR390_GAIN_1);
      ltr390.setResolution(LTR390_RESOLUTION_20BIT);
      ltr390.setMode(LTR390_MODE_ALS);
    }
  }
}

void updateAlldata() {
  // DHT11
  float* dhtData = dhtSensorRead();
  humidity = dhtData[0];
  temperature = dhtData[1];

  // CO2
  airQuality = gasSensor.getPPM();
  // airQuality = gasSensor.getCorrectedPPM(temperature, humidity);

  // LTR390
  updateLtr390Data();
}

void uploadAllData() {
  // {
  //   String payload(airQuality);
  //   mqtt.publish(TOPIC_AIR, payload.c_str());
  // }

  // {
  //   String payload(humidity);
  //   mqtt.publish(TOPIC_HUMIDITY, payload.c_str());
  // }

  // {
  //   String payload(temperature);
  //   mqtt.publish(TOPIC_TEMPERATURE, payload.c_str());
  // }

  // {
  //   String payload(lightLevel);
  //   mqtt.publish(TOPIC_LIGHT, payload.c_str());
  // }

  // {
  //   String payload(uv);
  //   mqtt.publish(TOPIC_UV, payload.c_str());
  // }

  // JSON
  {
    char jsonData[128] = {};
    snprintf(
      jsonData,
      128,
      "{\"temperature\": \"%.0f\", \"humidity\": \"%.0f\", \"airQuality\": \"%.0f\", \"uv\": \"%d\", \"light\": \"%d\"}",
      temperature, humidity, airQuality, uv, lightLevel);
    String payload(jsonData);
    mqtt.publish(TOPIC_JSON, payload.c_str());
  }
}

void uploadAllZeroData() {
  char jsonData[128] = {};
  snprintf(
    jsonData,
    128,
    "{\"temperature\": \"0\", \"humidity\": \"0\", \"airQuality\": \"0\", \"uv\": \"0\", \"light\": \"0\"}");
  String payload(jsonData);
  mqtt.publish(TOPIC_JSON, payload.c_str());
  // Serial.println(jsonData);
}

// * Debug *
void debugprintData() {
  Serial.printf("rzero: %f\n", gasSensor.getRZero());
  Serial.printf("rzero: %f\n", gasSensor.getCorrectedRZero(temperature, humidity));
  Serial.printf("ppm: %f\n", airQuality);
  Serial.printf("C ppm: %f\n", gasSensor.getCorrectedPPM(temperature, humidity));
  Serial.printf("humidity: %f\n", humidity);
  Serial.printf("temperature: %f\n", temperature);
  Serial.printf("Light: %d\n", lightLevel);
  Serial.printf("UVI: %d\n", uv);
  Serial.printf("UV raw: %d\n", uvRaw);
  Serial.printf("do: %d\n", digitalRead(MQ135_DO_PIN));
}

void uploadDebug() {

  char temp[256] = {};
  snprintf(
    temp,
    256,
    "temperature:%f \nhumidity%f \nairQuality: %f, %f \nuv:%d, %d \nlightLevel: %d \nr0: %f, %f",
    temperature, humidity, gasSensor.getPPM(), gasSensor.getCorrectedPPM(temperature, humidity), uv, uvRaw, lightLevel, gasSensor.getRZero(), gasSensor.getCorrectedRZero(temperature, humidity));
  String payload(temp);
  mqtt.publish(TOPIC_DEBUG, payload.c_str());
}
