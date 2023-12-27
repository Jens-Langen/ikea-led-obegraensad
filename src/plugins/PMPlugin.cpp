#include "plugins/PMPlugin.h"
#include <math.h>

int secondsToWait = 5;

enum DP_Type
{
  PM2_5,
  PM10
};

enum state
{
  text25,
  value25,
  indicator25,
  text10,
  value10,
  indicator10

};

enum indicator_typ
{
  indicator_neutral,
  indicator_good,
  indicator_bad
};

int currentState = text25;

String dps[2] = {"http://192.168.190.70:8087/getPlainValue/alias.0.Sensoren.PM.0.SDS_P2",
                 "http://192.168.190.70:8087/getPlainValue/alias.0.Sensoren.PM.0.SDS_P1"
                };

void PMPlugin::setup()
{

  // loading screen
  Screen.clear();
  currentStatus = LOADING;
  Screen.setPixel(4, 7, 1);
  Screen.setPixel(5, 7, 1);
  Screen.setPixel(7, 7, 1);
  Screen.setPixel(8, 7, 1);
  Screen.setPixel(10, 7, 1);
  Screen.setPixel(11, 7, 1);
  this->lastUpdate = millis();
  // this->update();
  currentStatus = NONE;
}

void PMPlugin::loop()
{
  if (millis() >= this->lastUpdate + (1000 * secondsToWait))
  {
    Screen.clear();
    switch (currentState)
    {

    case text25:
      this->progress(0);
      showText(PM2_5);
      currentState = value25;
      break;
    case value25:
      this->progress(1);
      this->update(PM2_5);
      currentState = indicator25;
      break;
    case indicator25:
      this->progress(2);
      this->showIndicator(PM2_5);
      currentState = text10;
      break;
    case text10:
      this->progress(3);
      showText(PM10);
      currentState = value10;
      break;
    case value10:
      this->progress(4);
      this->update(PM10);
      currentState = indicator10;
      break;
    case indicator10:
      this->progress(5);
      this->showIndicator(PM10);
      currentState = text25;
      break;
    }

    this->lastUpdate = millis();
    // Serial.println("updating PM");
  };
}

const char *PMPlugin::getName() const
{
  return "Particulate Matter";
}

void PMPlugin::update(int typ)
{

  int p = this->getData(typ);
  if (typ == PM2_5)
    currentPM25 = p;
  if (typ == PM10)
    currentPM10 = p;

  int h = (p - p % 100) / 100;
  int d = (p - p % 10) / 10;
  int e = (p % 10);

  if (p > 100)
    Screen.drawCharacter(0, 4, Screen.readBytes(system6x7[48 + h]), 8, Screen.getCurrentBrightness());
  if (p > 10)
    Screen.drawCharacter(3, 4, Screen.readBytes(system6x7[48 + d]), 8, Screen.getCurrentBrightness());
  Screen.drawCharacter(10, 4, Screen.readBytes(system6x7[48 + e]), 8, Screen.getCurrentBrightness());
}

int PMPlugin::getData(int typ)
{

  String url = dps[typ];

#ifdef ESP32
  http.begin(url);
#endif
#ifdef ESP8266
  http.begin(wiFiClient, url);
#endif

  int code = http.GET();

  if (code == HTTP_CODE_OK)
  {
    String res = http.getString();
    int iRes = round(res.toFloat() + 0.5);

    return iRes;
  }
  else
  {
    return -1;
  }
}

void PMPlugin::showText(int typ)
{

  Screen.drawCharacter(2, 1, Screen.readBytes(system6x7[80]), 8, Screen.getCurrentBrightness()); // P
  Screen.drawCharacter(9, 1, Screen.readBytes(system6x7[77]), 8, Screen.getCurrentBrightness()); // M

  if (typ == PM2_5)
  {
    Screen.drawCharacter(0, 9, Screen.readBytes(system6x7[50]), 8, Screen.getCurrentBrightness());
    Screen.drawCharacter(6, 9, Screen.readBytes(system6x7[46]), 8, Screen.getCurrentBrightness());
    Screen.drawCharacter(10, 9, Screen.readBytes(system6x7[53]), 8, Screen.getCurrentBrightness());
  }
  else if (typ == PM10)
  {
    Screen.drawCharacter(2, 9, Screen.readBytes(system6x7[49]), 8, Screen.getCurrentBrightness());
    Screen.drawCharacter(9, 9, Screen.readBytes(system6x7[48]), 8, Screen.getCurrentBrightness());
  }
}

void PMPlugin::progress(int value)
{
  // Screen.drawLine(0,0,15,0,true,50);
  Screen.setPixel(value * 2, 0, true, 255);
  Screen.setPixel(value * 2 + 1, 0, true, 255);
}

void PMPlugin::showIndicator(int typ)
{
  int state = indicator_bad;
  if (typ == PM2_5)
  {
    if (currentPM25 <= 25)
      state = indicator_neutral;
    if (currentPM25 <= 10)
      state = indicator_good;
  }

  if (typ == PM10)
  {
    if (currentPM10 <= 40)
      state = indicator_neutral;
    if (currentPM10 <= 20)
      state = indicator_good;
  }
  Serial.println(typ);
  Serial.println(state);

  Screen.drawCharacter(0, 2, Screen.readBytes(smileyIcons[state]), 16, Screen.getCurrentBrightness());
}
