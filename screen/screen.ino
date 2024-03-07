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


#include <WiFi.h>
#include "PubSubClient.h"
// #include "time.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <HCSR04.h>

#include "hardwareConfig.h"
#include "networkConfig.h"
#include "screen.h"


// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
// Wifi
WiFiClient wifiClient;
PubSubClient mqtt(MQTT_BROKER, 1883, wifiClient);
// Ultrasonic
UltraSonicDistanceSensor distanceSensor(US_TRIGGER_PIN, US_ECHO_PIN);


// ** Setup **
void setup() {
  strncpy(aiRespond, "Loading...", 10);
  // Debug
  Serial.begin(115200);

  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_YELLOW_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);

  digitalWrite(LED_GREEN_PIN, LOW);
  analogWrite(LED_YELLOW_PIN, 48);

  // Set SDA and SCL
  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);

  // Creates a waiting loop that continues until a serial connection is established with the computer
  while (!Serial) {
    printf("Waiting for serial connection.");
  }

  // Check
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    // Do nothing
    for (;;)
      ;
  }

  // Setup screen
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  delay(1000);

  // Booting animation
  renderBootPage(1);
  delay(300);


  // * Connect *
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connecting to WiFi");
  display.println(WIFI_SSID);
  display.display();

  connect_wifi();
  delay(200);

  display.println("\nConnecting MQTT");
  display.println(MQTT_BROKER);
  display.display();

  connect_mqtt();
  // delay(300);

  // display.println("\nConnecting NTP");
  // display.display();
  // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer, "time.nist.gov");
  delay(1000);

  renderOneLineText((char*)"    Connected :)", 28);
  digitalWrite(LED_GREEN_PIN, HIGH);
  analogWrite(LED_YELLOW_PIN, 0);
  delay(1000);
}


// ** Loop **
void loop() {
  currentInternalTime = millis();

  // Memory usage
  // if (currentInternalTime - lastDebugLoop >= 2000) {
  //   Serial.println("[APP] Free memory: " + String(esp_get_free_heap_size()) + " bytes\n");
  //   lastDebugLoop = millis();
  // }

  // Reconnect if Wifi not connected
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_RED_PIN, HIGH);
    reconnect_wifi();
  }

  // Reconnect if MQTT not connected
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


  // check for incoming subscribed topics
  mqtt.loop();


  // Get data from sensor
  switchStatus = digitalRead(SWITCH_PIN);


  updateUltrasonic();
  if ((currentInternalTime - lastDistanceLoop >= 2000)) {
    showLedSittingStatus();
    uploadSitting();
    lastDistanceLoop = millis();
  }


  // Preparing to rendering new frame
  display.clearDisplay();
  updateCurrentPage();

  showPagination = true;

  // Page rendering
  switch (currentPage) {
    case 1:
      renderChartPage((int)temperature, (int)humidity, uv, lightLevel, airQuality);
      break;
    case 2:
      renderDhtPage(temperature, humidity);
      break;
    case 3:
      renderLightPage(uv, lightLevel);
      break;
    case 4:
      renderAirPage(airQuality);
      break;
    case 5:
      if (isLoading) {
        display.setCursor(0, 0);
        display.println("Analyzing...");
        break;
      }
      renderMultiLineTextScroll(aiRespond, 10, 12, true);
      break;
    case CALIBRATE_PAGE:
      showPagination = false;
      renderSetupPage();
      break;
    case STAND_PAGE:
      showPagination = false;
      renderStandPage();

      // Siren LED
      if (millis() - lastLedSiren > 100) {
        switch (ledNum) {
          case 1:
            digitalWrite(LED_GREEN_PIN, HIGH);
            analogWrite(LED_YELLOW_PIN, 0);
            digitalWrite(LED_RED_PIN, LOW);
            break;
          case 2:
            digitalWrite(LED_GREEN_PIN, LOW);
            analogWrite(LED_YELLOW_PIN, 255);
            digitalWrite(LED_RED_PIN, LOW);
            break;
          case 3:
            digitalWrite(LED_GREEN_PIN, LOW);
            analogWrite(LED_YELLOW_PIN, 0);
            digitalWrite(LED_RED_PIN, HIGH);
            break;
        }
        // Switch LED
        ledNum = (ledNum >= 3) ? 1 : ledNum + 1;
        lastLedSiren = millis();
      }
      break;

    default:
      printf("switch case error %d", currentPage);
  }

  if (showPagination) {
    renderPagination(maxPage, currentPage, radius, margin, false);
  }


  delay(16);

  // Render
  display.display();
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
  mqtt.subscribe(TOPIC_AI);
  mqtt.subscribe(TOPIC_TEMPERATURE);
  mqtt.subscribe(TOPIC_HUMIDITY);
  mqtt.subscribe(TOPIC_AIR);
  mqtt.subscribe(TOPIC_UV);
  mqtt.subscribe(TOPIC_LIGHT);
  mqtt.subscribe(TOPIC_TIME);
  mqtt.subscribe(TOPIC_SITTING_RESET);
  // mqtt.subscribe(TOPIC_DEBUG);

  // Tell Node-RED ESP32 startup status
  String payload("restarted");
  mqtt.publish(TOPIC_SITTING_RESET, payload.c_str());

  printf("MQTT broker reconnected.\n");
}

void reconnectMQTT() {
  printf("Reconnecting to MQTT broker at %s.\n", MQTT_BROKER);
  if (mqtt.connect("", MQTT_USER, MQTT_PASS)) {

    mqtt.setCallback(mqtt_callback);
    mqtt.subscribe(TOPIC_AI);
    mqtt.subscribe(TOPIC_TEMPERATURE);
    mqtt.subscribe(TOPIC_HUMIDITY);
    mqtt.subscribe(TOPIC_AIR);
    mqtt.subscribe(TOPIC_UV);
    mqtt.subscribe(TOPIC_LIGHT);
    mqtt.subscribe(TOPIC_TIME);
    mqtt.subscribe(TOPIC_SITTING_RESET);
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
  // printf("Callback: {topic: %s, payload: %s}\n", topic, payload);

  if (strcmp(topic, TOPIC_AI) == 0) {
    strncpy(aiRespond, (char*)payload, length);
    isLoading = false;
    // printf("Callback: {topic: %s, payload: %s}\n", topic, payload);
  } else if (strcmp(topic, TOPIC_TEMPERATURE) == 0) {
    temperature = atof((char*)payload);
  } else if (strcmp(topic, TOPIC_TIME) == 0) {
    currentUnixTime = atol((char*)payload);
  } else if (strcmp(topic, TOPIC_HUMIDITY) == 0) {
    humidity = atof((char*)payload);
  } else if (strcmp(topic, TOPIC_AIR) == 0) {
    airQuality = atoi((char*)payload);
  }
  // OLED Screen
  else if (strcmp(topic, TOPIC_UV) == 0) {
    uv = atoi((char*)payload);
  } else if (strcmp(topic, TOPIC_LIGHT) == 0) {
    lightLevel = atoi((char*)payload);
  } else if (strcmp(topic, TOPIC_SITTING_RESET) == 0) {
    if (strcmp((char*)payload, "stand") == 0) {
      currentPage = STAND_PAGE;
    }
    // Stand up -> Reset time
    else if (strcmp((char*)payload, "reset") == 0) {
      currentPage = 1;
      sittingStart = 1;
      String payload("reseted");
      mqtt.publish(TOPIC_SITTING_RESET, payload.c_str());
    }
    // Go back to sit -> start reminder
    else if (strcmp((char*)payload, "continue") == 0) {
      if (!startingUp) {
        sittingStart = currentUnixTime;
        currentPage = 1;
      }
    }
  }
  // else if (strcmp(topic, TOPIC_DEBUG) == 0) {  /////////////
  //   sittingDistance = atof((char*)payload);
  // }
}

void uploadSitting() {
  // JSON
  {
    char jsonData[256] = {};
    snprintf(
      jsonData,
      128,
      "{\"isSitting\" : \"%d\", \"sittingStart\": \"%d\", \"sittingDistance\": \"%f\", \"currentDistance\": \"%f\"}",
      isSitting, sittingStart, sittingDistance, currentDistance);
    String payload(jsonData);
    mqtt.publish(TOPIC_SITTING_JSON, payload.c_str());
  }
}

void showLedSittingStatus() {
  if ((isSitting) & (currentPage != 7)) {
    digitalWrite(LED_GREEN_PIN, HIGH);
    analogWrite(LED_YELLOW_PIN, 0);
    digitalWrite(LED_RED_PIN, LOW);
  } else {
    digitalWrite(LED_GREEN_PIN, LOW);
    analogWrite(LED_YELLOW_PIN, 48);
    digitalWrite(LED_RED_PIN, LOW);
  }
}


// * Togling *

// prevent long press
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

bool updateCurrentPage() {
  if (checkToggling()) {
    if (currentPage == CALIBRATE_PAGE) {
      // calibratingDistance = false;
      sittingStart = currentUnixTime;
    }
    startingUp = false;
    currentPage = (currentPage >= maxPage) ? 1 : currentPage + 1;
    return true;
  }
  return false;
}

void updateUltrasonic() {
  currentDistance = distanceSensor.measureDistanceCm();
  if ((currentDistance - sittingDistance >= 30.0) || currentDistance < 0) {
    isSitting = false;

  } else {
    isSitting = true;
  }
}

// * Element Renderers*

void renderOneLineText(char text[], int startY) {
  display.clearDisplay();
  display.setCursor(0, startY);
  display.println(text);
  display.display();
}

// New line when text is to long for one line
void renderMultiLineText(char text[], int startY, int gapY, bool renderNow) {
  // printf("%s", text);
  display.clearDisplay();

  int lenght = strlen(text);
  int remainingChar = lenght;
  int cursorYCoordinate = startY;
  char tempString[23] = "";

  // Cut to chunks
  for (int i = 0; i < lenght; i += maxCharPerLine) {
    display.setCursor(0, cursorYCoordinate);

    if (remainingChar <= maxCharPerLine) {
      strncpy(tempString, text + i, remainingChar);
      tempString[remainingChar] = '\0';
    } else {
      strncpy(tempString, text + i, maxCharPerLine);
      tempString[maxCharPerLine + 1] = '\0';
      remainingChar -= maxCharPerLine;
    }
    // printf("render: %s\n", tempString);
    display.println(tempString);
    cursorYCoordinate += gapY;
  }
  if (renderNow) {
    display.display();
  }

  return;
}

// Vertical scroll for vertical overflow text
void renderMultiLineTextScroll(char text[], int startY, int gapY, bool pagination) {
  // printf("%s", text);
  display.clearDisplay();

  int scrollNumber = 0;  // Number of top render line on screen
  int lenght = strlen(text);
  int maxTextLine = (lenght / maxCharPerLine) + 5;
  uint32_t lastLoopTime = millis();

  // Cut to chunks
  while (scrollNumber <= maxTextLine * textHeight) {
    mqtt.loop();
    // Realtime switch check
    updateToggleStatus();
    if (updateCurrentPage()) {
      return;
    }

    // Render delays
    if (millis() - lastLoopTime <= 60) {
      continue;
    }

    renderMultiLineText(text, startY, gapY, false);

    if (pagination && (scrollNumber >= ((maxTextLine - 3) * textHeight))) {
      renderPagination(maxPage, currentPage, radius, margin, false);
    }

    renderScrollBar(maxTextLine * textHeight, scrollNumber);

    display.display();

    startY--;
    scrollNumber++;
    lastLoopTime = millis();
  }

  return;
}

void renderPagination(int maxPage, int currentPage, int radius, int margin, bool renderNow) {
  int diameter = radius * 2;
  int usedY = ((diameter + margin) * maxPage) - margin;  // equal to (diameter * maxPage) + (margin * (maxPage - 1))แ
  int startX = (SCREEN_WIDTH - usedY) / 2;
  int startY = SCREEN_HEIGHT - diameter;

  for (int i = 0; i < maxPage; i++) {
    const int offsetX = (i == 0) ? 0 : ((i * (diameter + margin)));

    if (i + 1 == currentPage) {
      display.fillCircle((startX + offsetX), startY, (radius + 1), WHITE);
      continue;
    }

    display.drawCircle((startX + offsetX), startY, radius, WHITE);
  }

  if (renderNow) {
    display.display();
  }

  return;
}

void renderScrollBar(int maxStep, int currentStep) {
  int barHeight = 12;
  int barY = (currentStep / (float)maxStep) * (SCREEN_HEIGHT - barHeight);

  display.fillRoundRect(SCREEN_WIDTH - 4, barY, 4, barHeight, 2, WHITE);
}

// * // Page Renderers *

void renderBootPage(int loadingDelay) {
  const int startX = 12;

  display.clearDisplay();

  display.setCursor(31, 9);
  display.println("Starting up");
  display.drawRoundRect(startX, 30, 101, 12, 5, WHITE);
  display.display();
  delay(500);

  for (int i = 0; i <= 100; i += 2) {
    display.clearDisplay();

    char strI[10] = { 0 };
    sprintf(strI, "%d%%", i);

    // text
    display.setCursor(31, 9);
    display.println("Starting up");
    display.setCursor((i + startX - 12), 50);
    display.println(strI);

    // Progress bar
    display.drawRoundRect(startX, 30, 101, 12, 5, WHITE);
    display.fillRoundRect(startX, 30, (i), 12, 5, WHITE);

    // Render
    display.display();
    delay(loadingDelay);
  }

  return;
}

void renderSetupPage() {
  calibratingDistance = true;

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Setup distance sensor");
  display.drawRoundRect(12, 21, 101, 12, 5, WHITE);




  sittingDistance = currentDistance;


  if ((0.0 < sittingDistance) && (sittingDistance < 60.0)) {
    display.setCursor(52, 41);
    display.print("Good");
    display.setCursor(28, 57);
    display.println("Press button");
    // display.fillRoundRect(12, 30, 100, 12, 5, WHITE);
  } else if ((0.0 < sittingDistance) && (sittingDistance < 100.0)) {
    display.setCursor(57, 41);
    display.println("Ok");
    // display.fillRoundRect(12, 66, 100, 12, 5, WHITE);
  } else {
    display.setCursor(25, 41);
    display.println("Not detected!");
    // display.fillRoundRect(12, 33, 100, 12, 5, WHITE);
  }

  int w = (200 - (float)sittingDistance) / 2;
  if ((w < 0) || (sittingDistance == -1)) {
    w = 0;
  }
  display.fillRoundRect(12, 21, w, 12, 5, WHITE);

  display.display();
}

void renderChartPage(int temperature, int humidity, int uv, int lightLevel, int airQuality) {
  const int chartHeight = 40;

  int temperatureChartHeight = (temperature / 45.0f) * chartHeight;
  int humidityChartHeight = (humidity / 100.0f) * chartHeight;
  int uvChartHeight = (uv / 11.0f) * chartHeight;
  int lightLevelChartHeight = (lightLevel / 50000.0f) * chartHeight;
  int airQualityChartHeight = (airQuality / 2000.0f) * chartHeight;

  if (lightLevelChartHeight > chartHeight) {
    lightLevelChartHeight = chartHeight;
  }
  if (airQualityChartHeight > chartHeight) {
    airQualityChartHeight = chartHeight;
  }

  // display.drawRoundRect(6, 0, 10, 50, 3, WHITE);
  // display.drawRoundRect(32, 0, 10, 50, 3, WHITE);
  // display.drawRoundRect(58, 0, 10, 50, 3, WHITE);
  // display.drawRoundRect(84, 0, 10, 50, 3, WHITE);
  // display.drawRoundRect(110, 0, 10, 50, 3, WHITE);

  for (int i = 6; i <= 111; i += 26) {
    display.drawRoundRect(i, 0, 10, 40, 3, WHITE);
  }

  // Temperature
  display.fillRoundRect(6, (chartHeight - temperatureChartHeight), 10, temperatureChartHeight, 3, WHITE);
  display.setCursor(5, 45);
  display.print(temperature);
  display.cp437(true);
  display.write(248);

  // Humidity
  display.fillRoundRect(32, (chartHeight - humidityChartHeight), 10, humidityChartHeight, 3, WHITE);
  display.setCursor(30, 45);
  display.print(humidity);
  display.print("%");

  // UV
  display.fillRoundRect(58, (chartHeight - uvChartHeight), 10, uvChartHeight, 3, WHITE);
  display.setCursor(58, 45);
  display.print(uv);

  // Light Level
  display.fillRoundRect(84, (chartHeight - lightLevelChartHeight), 10, lightLevelChartHeight, 3, WHITE);
  display.setCursor(72, 45);
  if (lightLevel < 10000) {
    display.setCursor(76, 45);
  }
  if (lightLevel < 1000) {
    display.setCursor(80, 45);
  }
  display.print(lightLevel);
  display.setCursor(82, 54);
  display.print("lx");

  // Air Quality
  display.fillRoundRect(110, (chartHeight - airQualityChartHeight), 10, airQualityChartHeight, 3, WHITE);
  display.setCursor(101, 45);
  if (airQuality < 1000) {
    display.setCursor(108, 45);
  }
  display.print(airQuality);
  display.setCursor(105, 54);
  display.print("ppm");
}

void renderDhtPage(float temperature, float humidity) {
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Temperature: ");
  display.setTextSize(2);
  display.setCursor(0, 10);
  display.print(temperature);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(248);
  display.setTextSize(2);
  display.print("C");

  // display humidity
  display.setTextSize(1);
  display.setCursor(0, 30);
  display.print("Humidity: ");
  display.setTextSize(2);
  display.setCursor(0, 40);
  display.print(humidity);
  display.print(" %");

  display.setTextSize(1);
}

void renderLightPage(int uv, int lightLevel) {
  // display UV
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("UV Index: ");
  display.setTextSize(2);
  display.setCursor(0, 10);
  display.print(uv);
  display.print(" ");

  // display Light Level
  display.setTextSize(1);
  display.setCursor(0, 30);
  display.print("Light Level: ");
  display.setTextSize(2);
  display.setCursor(0, 40);
  display.print(lightLevel);
  display.print(" lx");

  display.setTextSize(1);
}

void renderAirPage(int airQuality) {
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("CO2 Level: ");
  display.setTextSize(2);
  display.setCursor(0, 10);
  display.print(airQuality);
  display.print(" ppm");

  display.setTextSize(1);
}

void renderStandPage() {
  display.setTextSize(2);
  display.setCursor(10, 26);
  display.print("Stand up!");

  display.setTextSize(1);
}