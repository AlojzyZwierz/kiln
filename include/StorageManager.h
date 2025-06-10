#pragma once

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include "CurveManager.h"    // zakładamy, że masz taką strukturę
#include "SettingsManager.h" // zakładamy, że masz taką strukturę
#include "WifiCredentials.h"
class SettingsManager;

// zakładamy, że masz instancję CurveManager

class StorageManager
{
public:
  static bool begin();

  // CURVE
  static void saveCurve(CurveManager &curveManager, int index);
  static void loadCurve(CurveManager &curveManager, int index);

  static void appendErrorLog(const String &message);
  static String loadLastError();

  // Power
  static void saveSettings();
  static void loadSettings();

  static void saveWiFiCredentials(WiFiCredentials &creds);
  static WiFiCredentials loadWiFiCredentials();

  static void format(); // czyści SPIFFS
  // void setCurveManager(CurveManager& manager);

  template <typename T>
  static void save(const char *path, const T &data)
  {
    File file = SPIFFS.open(path, FILE_WRITE);
    if (!file)
    {
      Serial.println("writing erroarrrrr!");
      return;
    }
    file.write((const uint8_t *)&data, sizeof(T));
    file.close();
    Serial.println("data written!");
  }

  template <typename T>
  static bool load(const char *path, T &data)
  {
    File file = SPIFFS.open(path, FILE_READ);
    if (!file)
    {
      Serial.println("reading erroroarrrr!");
      return false;
    }

    file.read((uint8_t *)&data, sizeof(T));
    file.close();
    Serial.println("Dane wczytane!");
    return true;
  }

private:
};
