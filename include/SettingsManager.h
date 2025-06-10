#pragma once
#include <stddef.h> // for size_t
#include <Arduino.h>

class StorageManager;

struct Settings
{
    float pid_kp = 1.0f;
    float pid_ki = 1.0f;
    float pid_kd = 1.1f;
    unsigned long pidIntervalMs = 5000;
    unsigned long heatingCycleMs = 7400;
    float kilnPower = 2.8f; // kW
    float unitCost = 0.9f;  // PLN/kWh
};
enum StepMode
{
    FIXED,  // krok absolutny, np. +500ms
    PERCENT // krok procentowy, np. +10% wartości
};
// Entry w GUI – jedna edytowalna wartość
struct SettingEntry
{
    const char *name;
    void *valuePtr;
    enum ValueType
    {
        FLOAT,
        ULONG
    } type;
    float minValue;
    float maxValue;
    float step;        // Używane tylko w trybie FIXED
    StepMode stepMode; // FIXED lub PERCENT
    float percentStep; // Używane tylko w trybie PERCENT
};
class SettingsManager
{
private:
    Settings settings;
    SettingsManager() = default;

    static SettingEntry entries[];

public:
    static SettingsManager &get()
    {
        static SettingsManager instance;
        return instance;
    }

    SettingsManager(const SettingsManager &) = delete;

    const Settings &getSettings() const;
    void setSettings(const Settings &newSettings);

    SettingEntry *getSettingEntries();
    size_t getSettingEntryCount() const;
    void increase();
    void decrease();
    void next();
    void previous();
    const SettingEntry &getCurrentEntry() const;
};
