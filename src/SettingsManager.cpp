#include "SettingsManager.h"
#include "StorageManager.h"



void SettingsManager::setSettings(const Settings& newSettings){
    settings = newSettings;
    StorageManager::saveSettings();
    Serial.println("Settings saved: " + String(settings.pid_kp) + ", " + String(settings.pid_ki) + ", " + String(settings.pid_kd));
    Serial.println("Settings saved: " + String(settings.heatingCycleMs) + ", " + String(settings.kilnPower) + ", " + String(settings.unitCost));
}

const Settings& SettingsManager::getSettings() const { return settings; }
//Settings& getSettings() { return settings; }