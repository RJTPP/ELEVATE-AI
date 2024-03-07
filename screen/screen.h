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


#include "esp32-hal.h"
#include <stdint.h>

enum specialPage {
  CALIBRATE_PAGE = 6,
  STAND_PAGE = 7
};

// ** Variable **

// Time
long currentUnixTime = 0;
uint32_t currentInternalTime = millis();

// Connection
// uint32_t lastWifiAttempt = millis();
bool connectInProgress = false;
uint32_t lastMqttAttempt = millis();

// Max per line: 21
// Max line: 7
// Max char: 147
const int textWidth = 6;
const int textHeight = 8;
const int maxCharPerLine = ((int)(SCREEN_WIDTH / textWidth)) - 1;  //21 - 1

// Switch
int switchStatus = 1;
bool isToggling = false;
bool isLongPress = false;

// Pagination
const int radius = 2;
const int margin = 3;
const int maxPage = 5;
int currentPage = CALIBRATE_PAGE;
bool showPagination = false;

// Data
float temperature = 0;
float humidity = 0;
int uv = 0;
int lightLevel = 0;
int airQuality = 0;
char aiRespond[2048];
bool isLoading = true;
uint32_t lastDebugLoop = millis();
uint32_t lastDistanceLoop = millis();

// time
// const char* ntpServer = "pool.ntp.org";
// const long gmtOffset_sec = 0;
// const int daylightOffset_sec = 3600;

// sitting data
bool startingUp = true;
float sittingDistance = 0.0;
float currentDistance = 0.0;
bool isSitting = true;
bool calibratingDistance = true;
unsigned long sittingStart = 1;
int ledNum = 1;
uint32_t lastLedSiren = millis();


// ** Function Delaration **

// MQTT
void connect_wifi();
void reconnect_wifi();
void connect_mqtt();
void reconnectMQTT();
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void uploadSitting();

// unsigned long getCurrentTime();

// Togling
void updateToggleStatus();
bool checkToggling();
bool updateCurrentPage();

// Sitting
void updateUltrasonic();
void showLedSittingStatus();

// Element Renderers
void renderOneLineText(char text[], int startY);
void renderMultiLineText(char text[], int startY, int gapY, bool rendetNow);
void renderMultiLineTextScroll(char text[], int startY, int gapY, bool pagination);
void renderPagination(int maxPage, int currentPage, int radius, int margin, bool renderNow);
void renderScrollBar(int maxStep, int currentStep);

// Page Renderers
void renderBootPage(int loadingDelay);
void renderSetupPage();
void renderChartPage(int temperature, int humidity, int uv, int lightLevel, int airQuality);
void renderDhtPage(float temperature, float humidity);
void renderLightPage(int uv, int lightLevel);
void renderAirPage(int airQuality);
void renderStandPage();
