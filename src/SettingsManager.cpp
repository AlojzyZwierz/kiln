#include "SettingsManager.h"
#include <algorithm> // std::min, std::max

// ⚙️ Definicja tablicy entries[]
SettingEntry SettingsManager::entries[] = {
    {"PID Kp", &get().settings.pid_kp, SettingEntry::FLOAT, 0.0f, 1000.0f, 0.1f, PERCENT, 10.0f},
    {"PID Ki", &get().settings.pid_ki, SettingEntry::FLOAT, 0.0f, 1000.0f, 0.1f, PERCENT, 10.0f},
    {"PID Kd", &get().settings.pid_kd, SettingEntry::FLOAT, 0.0f, 1000.0f, 0.1f, PERCENT, 10.0f},
    {"PID Interval", &get().settings.pidIntervalMs, SettingEntry::ULONG, 100, 30000, 1000, FIXED, 0.0f},
    {"Cycle Time", &get().settings.heatingCycleMs, SettingEntry::ULONG, 100, 30000, 1000, FIXED, 0.0f},
    {"Power [kW]", &get().settings.kilnPower, SettingEntry::FLOAT, 0.1f, 10.0f, 0.1f, FIXED, 10.0f},
    {"Cost [PLN]", &get().settings.unitCost, SettingEntry::FLOAT, 0.1f, 10.0f, 0.05f, FIXED, 10.0f},
};

const Settings &SettingsManager::getSettings() const
{
    return settings;
}

void SettingsManager::setSettings(const Settings &newSettings)
{
    settings = newSettings;
}

SettingEntry *SettingsManager::getSettingEntries()
{
    return entries;
}

size_t SettingsManager::getSettingEntryCount() const
{
    return sizeof(entries) / sizeof(entries[0]);
}

// ----------------- 🔁 Iteracja po entry -----------------

size_t currentIndex = 0;

void SettingsManager::next()
{
    currentIndex = (currentIndex + 1) % getSettingEntryCount();
}

void SettingsManager::previous()
{
    currentIndex = (currentIndex + getSettingEntryCount() - 1) % getSettingEntryCount();
}

const SettingEntry &SettingsManager::getCurrentEntry() const
{
    return entries[currentIndex];
}

// ----------------- 🔼 Zmiana wartości -----------------

void SettingsManager::increase()
{
    auto &entry = entries[currentIndex];

    switch (entry.type)
    {
    case SettingEntry::FLOAT:
    {
        float *ptr = static_cast<float *>(entry.valuePtr);
        float stepValue = entry.stepMode == FIXED ? entry.step : (*ptr) * (entry.percentStep / 100.0f);
        *ptr = std::min(*ptr + stepValue, entry.maxValue);
        break;
    }
    case SettingEntry::ULONG:
    {
        auto *ptr = static_cast<unsigned long *>(entry.valuePtr);
        float stepValue = entry.stepMode == FIXED ? entry.step : (*ptr) * (entry.percentStep / 100.0f);
        long newValue = std::min((long)*ptr + static_cast<long>(stepValue), static_cast<long>(entry.maxValue));
        *ptr = std::max(newValue, 0L); // Ensure it doesn't go below minValue
     //   Serial.println("New value: " + String(*ptr) + " Max: " + String(entry.maxValue) + " newvalue: " + String(newValue));
        break;
    }
    }
}

void SettingsManager::decrease()
{
    auto &entry = entries[currentIndex];

    switch (entry.type)
    {
    case SettingEntry::FLOAT:
    {
        float *ptr = static_cast<float *>(entry.valuePtr);
        float stepValue = entry.stepMode == FIXED ? entry.step : (*ptr) * (entry.percentStep / 100.0f);
        *ptr = std::max(*ptr - stepValue, entry.minValue);
        break;
    }
    case SettingEntry::ULONG:
    {
        auto *ptr = static_cast<unsigned long *>(entry.valuePtr);
        float stepValue = entry.stepMode == FIXED ? entry.step : (*ptr) * (entry.percentStep / 100.0f);
        long newValue = std::min((long)*ptr - static_cast<long>(stepValue), static_cast<long>(entry.maxValue));
        *ptr = std::max(newValue, 0L); // Ensure it doesn't go below minValue
      //  Serial.println("New value: " + String(*ptr) + " Max: " + String(entry.maxValue) + " newvalue: " + String(newValue));
        break;
    }
    }
}
