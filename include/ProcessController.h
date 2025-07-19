#pragma once

#include "Line.h"
#include "CurveManager.h"
#include "SettingsManager.h"
#include "HeatingController.h"
#include "SoundManager.h"
#include "TemperatureSensor.h"
#include "ResumeManager.h"
#include "StorageManager.h"
#include "FakeFurnace.h"
#include "MeasurementManager.h"

// #define SSR 9 // pin przekaźnika – ustaw wg własnych potrzeb

class ProcessController
{
public:
    static ProcessController &get()
    {
        static ProcessController instance;
        return instance;
    }
    void begin(CurveManager &cm, TemperatureSensor &sensor, HeatingController &h, EnergyUsageMeter &eMeter);
    void checkSegmentAdvance();
    // void updateHeating();
    void startFiring();
    // bool isRunning() const;
    void abort(const char *reason = nullptr);
    void applyPID();
    // int getCurrentSegmentIndex() const { return curveManager.getSegmentIndex(); }
    static float getMaxTemp(Curve c);
    void checkForErrors();
    float getExpectedTemp() const
    {
        if (SystemState::get().getMode() != SystemMode::Firing)
            return 0;
        float exp;
        if (segmentLine.a > 0)
            exp = min(segmentLine.y(millis()), curveManager->getSegmentTemp());
            else
            exp = max(segmentLine.y(millis()), curveManager->getSegmentTemp());
        // Serial.println("Expected temp: " + String(exp) );
        return exp;
    }
    // unsigned long getCurveStartTime()  { return curveStartTime; }
    unsigned long getProgramStartTime() const { return programStartTime; }
    float getProgramStartTemperature() const { return programStartTemperature; }
    unsigned long getSegmentStartTime() const { return segmentStartTime; }
    // unsigned long getSegmentEndTime() const { return segmentEndTime; }
    void setErrorCallback(std::function<void(const String &)> cb)
    {
        onError = cb;
    }
    float getP() { return prop; }
    float getI() { return integ; }
    float getD() { return deriv; }
    float getRatio() { return ratio; }
    float getCurrentTemp();
    float getCJTemp();
    unsigned long getStartTimeOffset()
    {
        // Serial.println("startTimeOffset: " + String(startTimeOffset));
        return startTimeOffset + ProcessController::get().getInitialSkipTime();
    }
    void adjustSkipTime();
    unsigned long getInitialSkipTime() { return initialSkipTime; }

private:
    std::function<void(const String &)> onError;
    void useSegment();
    void nextSegment();
    ProcessController() {}
    float epsilon = 0.00001f; //  - granica przy której uważamy prostą za poziomą

    CurveManager *curveManager;
    // CurveManager* sourceCurveManager ;
    TemperatureSensor *temperatureSensor; // = nullptr;
    HeatingController *heating;
    EnergyUsageMeter *euMeter;
    // MeasurementManager* measurementManager;

    // uint8_t currentSegmentIndex = 0;
    uint8_t editSegmentIndex = 0;
    unsigned long segmentStartTime = 0;
    unsigned long segmentEndTime = 0;
    unsigned long lastPidCheckTime = 0;
    // unsigned long curveStartTime = 0;
    unsigned long programStartTime = 0;
    float programStartTemperature; // czas zakończenia programu

    float startTemp = 0;
    float ratio = 0;
    float integral = 0;
    float lastError = 0;
    float maxSkipTemp = 0;
    unsigned long maxSkipTime = 0;

    float integ;
    float deriv;
    float prop;

    Line segmentLine;
    // bool running = false;

    void setHeaterPower(float ratio);
    void finishFiring();
    uint8_t determineStartSegment();
    bool IsHeatingStuckDuringSkipMode();
    unsigned long startTimeOffset;
    bool initialSegment;
    unsigned long initialSkipTime = 0;
};
