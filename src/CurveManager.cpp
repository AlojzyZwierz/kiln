#include "CurveManager.h"

CurveManager::CurveManager() {}



void CurveManager::loadOriginalCurve(const Curve &inputCurve, int index)
{
    Serial.println("load org cur");
    if (SystemState::get().isLocked())
    {
        Serial.println("Cannot load curve while firing");
        return;
    }
    originalCurve = inputCurve;
    bool zeroReached = false;
    for (size_t i = 0; i < curveElemsNo; i++)
    {
        if (originalCurve.elems[i].hTime == 0)
        {
            zeroReached = true;
        }
        if (zeroReached)
        {
            originalCurve.elems[i].hTime = 0;
            originalCurve.elems[i].endTemp = 100.0f;
        }
    }
    currentCurveIndex = index;
    adjustedCurve = genCurveWithFakeSkips(originalCurve);
}

void CurveManager::resetAdjustedCurve()
{
    adjustedCurve = originalCurve;
}

bool CurveManager::isValidIndex(char index) const
{
    return index < curveElemsNo && index >= 0;
}

const Curve &CurveManager::getOriginalCurve() const
{
    return originalCurve;
}

const Curve &CurveManager::getAdjustedCurve() const
{
    return adjustedCurve;
}

const int CurveManager::getcurrentCurveIndex() const
{
    return currentCurveIndex;
}


Curve CurveManager::getDefaultCurve()
{
    Curve defaultCurve;

    // Curve defaultCurve;
    defaultCurve.elems[0].endTemp = 94;
    defaultCurve.elems[1].endTemp = 94;
    defaultCurve.elems[2].endTemp = 149;
    defaultCurve.elems[3].endTemp = 149;
    defaultCurve.elems[4].endTemp = 571;
    defaultCurve.elems[5].endTemp = 571;
    defaultCurve.elems[6].endTemp = 1000;
    defaultCurve.elems[7].endTemp = 1000;

    defaultCurve.elems[0].hTime = 4500000;
    defaultCurve.elems[1].hTime = 7200000;
    defaultCurve.elems[2].hTime = 1800000;
    defaultCurve.elems[3].hTime = 1800000;
    defaultCurve.elems[4].hTime = 14400000;
    defaultCurve.elems[5].hTime = 1800000;
    defaultCurve.elems[6].hTime = 14400000;
    defaultCurve.elems[7].hTime = 0;

    return defaultCurve;
}

Curve CurveManager::genCurveWithFakeSkips(Curve &curve)
{
    Curve modified;
    bool zeroReached = false;
    for (int i = 0; i < curveElemsNo; ++i)
    {
        if (curve.elems[i].hTime == 0)
            zeroReached = true;
        if (!zeroReached)
        {
            float temperatureSpan = curve.elems[i].endTemp - (i == 0 ? 20.0f : curve.elems[i - 1].endTemp);
            modified.elems[i].hTime = (curve.elems[i].skip == 1 || curve.elems[i].skip == 2)
                                          ? (unsigned long)(temperatureSpan * curve.elems[i].endTemp * 4.2f)
                                          : curve.elems[i].hTime;
                                          Serial.println(String(temperatureSpan)+ " skiptime: " + String(modified.elems[i].hTime));
            modified.elems[i].endTemp = curve.elems[i].endTemp;
        }
        else
        {
            modified.elems[i].hTime = 0;
            modified.elems[i].endTemp = 350.0f; // Set to a high value to indicate no temperature
        }
    }
    return modified;
}

float CurveManager::getMaxTemp(Curve &c)
{
    float maxT = 0;
    for (int i = 0; i < curveElemsNo; i++)
    {
        if (c.elems[i].hTime == 0)
            break;
        if (c.elems[i].endTemp > maxT)
        {
            maxT = c.elems[i].endTemp;
        }
    }
    return maxT;
}
CurveManager CurveManager::clone() const
{
    CurveManager copy;
    copy.originalCurve = this->originalCurve;
    copy.adjustedCurve = this->adjustedCurve;
    copy.currentCurveIndex = this->currentCurveIndex;
    return copy;
}
void CurveManager::updateTemperature(char index, float newTemperature)
{
    if (SystemState::get().isLocked())
    {
        Serial.println("Cannot update temperature while firing");
        return;
    }
    if (isValidIndex(index))
    {
        originalCurve.elems[index].endTemp = newTemperature;
        adjustedCurve = genCurveWithFakeSkips(originalCurve);
    }
}
void CurveManager::updateTime(char index, unsigned long newDurationMs)
{
    if (SystemState::get().isLocked())
    {
        Serial.println("Cannot update time while firing");
        return;
    }
    if (isValidIndex(index))
    {
        originalCurve.elems[index].hTime = newDurationMs;
        adjustedCurve = genCurveWithFakeSkips(originalCurve); // Update adjusted curve after changing time
                                                              // Serial.println("Adjusted curve: " + adjustedCurve.toString());
        // Serial.println("Original curve: " + originalCurve.toString());
    }
}
void CurveManager::updateAdjustedCurve(char index, unsigned long newDurationMs)
{
    if (isValidIndex(index))
    {
        adjustedCurve.elems[index].hTime = newDurationMs;
        // Update adjusted curve after changing time
    }
}

void CurveManager::adjustSkipTime(float deltaTemp, float deltaTime, int index){
    float newTime = deltaTime* originalCurve.elems[index].endTemp/ deltaTemp;
    updateAdjustedCurve(index, newTime );
}
void CurveManager::adjustSkipTime(float deltaTemp, float deltaTime){
    adjustSkipTime(deltaTemp, deltaTime, currentCurveIndex);
}
float CurveManager::getHeatingSpeed() const
{
    float deltaTemp = originalCurve.elems[currentSegmentIndex].endTemp - (currentSegmentIndex == 0 ? 20.0f : originalCurve.elems[currentSegmentIndex - 1].endTemp);

    unsigned long deltaTime = originalCurve.elems[currentSegmentIndex].hTime - originalCurve.elems[currentSegmentIndex - 1].hTime;
    if (deltaTime == 0)
        return 0.0f; // Avoid division by zero
    return deltaTemp / (deltaTime / 3600000.0f); // Convert milliseconds to hours
}
