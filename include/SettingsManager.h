#pragma once
//#include "StorageManager.h"
class StorageManager;
struct Settings {
    float pid_kp = 1.0f;
    float pid_ki =1.0f;
    float pid_kd = 1.1f;
    unsigned long pidIntervalMs = 5000;
    unsigned long heatingCycleMs = 7400;
    float kilnPower = 2.8f; // kW
    float unitCost = 0.9f; // PLN/kWh

};

class SettingsManager {
private:
    Settings settings;
    SettingsManager() {}
public:
    static SettingsManager& get() {
        static SettingsManager instance;
        
        return instance;
    }

    const Settings& getSettings() const;
    //Settings& getSettings() { return settings; }
    void setSettings(const Settings& newSettings);
};
