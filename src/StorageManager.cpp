#include "StorageManager.h"

bool StorageManager::begin()
{
  //  Serial.println("Mounting SPIFFS...");
    if (!SPIFFS.begin(true))
    {
        Serial.println("SPIFFS Mount Failed");
        return false;
    }
   // Serial.println("SPIFFS mounted!");

    File root = SPIFFS.open("/");
    if (!root)
    {
        Serial.println("- failed to open directory");
        return false;
    }
    if (!root.isDirectory())
    {
        Serial.println(" - not a directory");
        return false;
    }
  //  Serial.println("SPIFFS root directory OK.");
    return true;
}

void StorageManager::format()
{
    SPIFFS.format();
}

void StorageManager::saveSettings()
{
    save("/settings.bin", SettingsManager::get().getSettings());
}
void StorageManager::loadSettings()
{
    Settings settings;
    load("/settings.bin", settings);
    SettingsManager::get().setSettings(settings);
}
void StorageManager::saveWiFiCredentials(WiFiCredentials &creds)
{
    save("/wifi_creds.bin", creds);
}

WiFiCredentials StorageManager::loadWiFiCredentials()
{
    WiFiCredentials creds;
    load("/wifi_creds.bin", creds);
    return creds;
}

void StorageManager::saveCurve(CurveManager &curveManager, int index)
{
    String path = "/prog" + String(index);

    save(path.c_str(), curveManager.getOriginalCurve());
    Serial.println("SAVE: " + curveManager.getOriginalCurve().toString());
}
void StorageManager::loadCurve(CurveManager &curveManager, int index)
{
    String path = "/prog" + String(index);
   
   
    Curve loadedCurve;
    if (!load(path.c_str(), loadedCurve))
    {

        Serial.println("Nie udalo sie wczytac krzywej, wczytano domyslna.");

        loadedCurve = CurveManager::getDefaultCurve();
        saveCurve(curveManager, index);
    }
    int maxtemp = CurveManager::getMaxTemp(loadedCurve);
    if (maxtemp > 1300 || maxtemp < 30)
    {
        Serial.println("Niepoprawna krzywa, max temp > 1300. Ustawiono domyślną.");
        loadedCurve = CurveManager::getDefaultCurve();
        saveCurve(curveManager, index);
    }
    curveManager.loadOriginalCurve(loadedCurve, index);
    //curveManager.setcurrentCurveIndex(index);
    //int iindex = curveManager.getcurrentCurveIndex();
    // Serial.println(String(iindex)+" LOAD: " + loadedCurve.toString());
}

void StorageManager::appendErrorLog(const String &message)
{
    File file = SPIFFS.open("/error.log", "a");
    if (!file)
    {
        Serial.println("Failed to open error.log");
        return;
    }

    // Dodaj timestamp jeśli masz np. millis() lub czas RTC:
    file.print(millis()); // Albo timestamp w inny sposób
    file.print(" ms: ");
    file.println(message);
    file.close();
}
String StorageManager::loadLastError()
{
    File file = SPIFFS.open("/last_error.log", "r");
    if (!file)
        return "";

    String message = file.readStringUntil('\n');
    file.close();
    return message;
}