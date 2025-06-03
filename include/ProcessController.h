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

//#define SSR 9 // pin przekaźnika – ustaw wg własnych potrzeb

class ProcessController {
public:
static ProcessController& get(){
    static ProcessController instance;
    return instance;
}
    void begin(CurveManager& cm, TemperatureSensor& sensor,  HeatingController& h);
    void checkSegmentAdvance();
    //void updateHeating();
    void startFiring();
    bool isRunning() const;
    void abort(const char* reason = nullptr);
    void applyPID();
    //int getCurrentSegmentIndex() const { return curveManager.getSegmentIndex(); }
    static float getMaxTemp(Curve c);
    float getExpectedTemp() const {
        float exp = segmentLine.y(millis());
        //Serial.println("Expected temp: " + String(exp) );
        return exp;
        
    }
    unsigned long getCurveStartTime()  { return curveStartTime; }
private:
    
    void useSegment();
    void nextSegment();
    ProcessController(){}
    float epsilon = 0.00001f; //  - granica przy której uważamy prostą za poziomą

    CurveManager* curveManager;
    //CurveManager* sourceCurveManager ;
    TemperatureSensor* temperatureSensor = nullptr;
    HeatingController* heating;
    //MeasurementManager* measurementManager;

    //uint8_t currentSegmentIndex = 0;
    uint8_t editSegmentIndex = 0;
    unsigned long segmentStartTime = 0;
    unsigned long segmentEndTime = 0;
    unsigned long lastPidCheckTime =0;
    unsigned long curveStartTime = 0;

    float startTemp = 0;
    float ratio = 0;
    float integral = 0;
    float lastError = 0;

    Line segmentLine;
    bool running = false;

    float getCurrentTemp();
    
    void setHeaterPower(float ratio);
    void finishFiring();
    uint8_t determineStartSegment(const Curve& curve, float currentTemp);

};
