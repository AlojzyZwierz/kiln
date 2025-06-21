#pragma once
#include <Arduino.h>
#include <Line.h>
#include <SystemState.h>
// #include <string>
struct Seg
{
    unsigned long hTime;
    float endTemp;
    String toString() const
    {
        return ("<" + String(hTime) + "::" + String(endTemp) + ">");
    }
};

constexpr int curveElemsNo = 25;

struct Curve
{
    Seg elems[curveElemsNo];
    String toString() const
    {
        String toReturn = "";
        for (int i = 0; i < curveElemsNo; i++)
        {
            // if (elems[i].hTime == 0) break;
            toReturn += " [" + String(i) + "] " + elems[i].toString() + "\n";
        }
        return toReturn;
    }
};

class CurveManager
{
public:
    // constexpr static int curveElemsNo = curveElemsN;
    CurveManager();
    // int getCurveCount() const { return curveElemsNo; }
    void loadOriginalCurve(const Curve &inputCurve);
    void resetAdjustedCurve();

    void updateTime(char index, unsigned long newDurationMs);
    void updateTemperature(char index, float newTemperature);
    void updateAdjustedCurve(char index, unsigned long newDurationMs);
    void adjustSkipTime(float deltaTemp, float deltaTime);
    void adjustSkipTime(float deltaTemp, float deltaTime, int index);

    const Curve &getOriginalCurve() const;
    const Curve &getAdjustedCurve() const;

    static float getMaxTemp(Curve &c);

    bool isValidIndex(char index) const;
    const int getcurrentCurveIndex() const;
    const void setcurrentCurveIndex(unsigned int index);

    static Curve getDefaultCurve();
    CurveManager clone() const;
    void setSegmentIndex(unsigned int index) { currentSegmentIndex = index; }
    int getSegmentIndex() const { return currentSegmentIndex; }

    unsigned long getSegmentTime() const { return originalCurve.elems[currentSegmentIndex].hTime; }
    float getSegmentTemp() const { return originalCurve.elems[currentSegmentIndex].endTemp; }
    bool hasNextSegment() const
    {
        return currentSegmentIndex + 1 < curveElemsNo && originalCurve.elems[currentSegmentIndex + 1].hTime != 0;
    }
    void nextSegment()
    {
        if (hasNextSegment())
        {
            currentSegmentIndex++;
        }
        else
        {
            Serial.println("No more segments available.");
        }
    }
    bool isSkip();
    bool isSkip(int index);
    unsigned long getSegmentStartTemperature(){return getSegmentStartTemperature(currentSegmentIndex); }
    unsigned long getSegmentStartTemperature(int index){return index == 0?20:originalCurve.elems[index-1].endTemp;}
private:
    int currentSegmentIndex = 0;
    Curve originalCurve;
    Curve adjustedCurve;
    unsigned int currentCurveIndex = 0;
    static Curve genCurveWithFakeSkips(Curve &curve);
};