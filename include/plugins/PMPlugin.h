#pragma once

#ifdef ESP32
#include <HTTPClient.h>
#endif
#ifdef ESP8266
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#endif
#include <ArduinoJson.h>
#include "PluginManager.h"

class PMPlugin : public Plugin
{
private:
  unsigned long lastUpdate = 0;
  HTTPClient http;
  void update(int typ);
  int getData(int typ);
  void showText(int typ);
  void progress(int value);
  int currentPM25;
  int currentPM10;
  void showIndicator(int typ);

public:
  void setup() override;
  void loop() override;
  const char *getName() const override;
};
