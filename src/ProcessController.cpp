#include "ProcessController.h"

// extern FakeFurnace furnace;

void ProcessController::begin(CurveManager &cm, TemperatureSensor &sensor, HeatingController &h, EnergyUsageMeter &eMeter)
{
    curveManager = &cm;
    temperatureSensor = &sensor;
    // measurementManager = &mm;
    heating = &h;
    euMeter = &eMeter;
}

void ProcessController::startFiring()
{
    Serial.println("Starting firing process...");
    euMeter->reset();
    if (SystemState::get().getMode() == SystemMode::Firing)
        return; // Jeśli już działa, nie rób nic
    SystemState::get().setMode(SystemMode::Firing);
    // Serial.println("System mode set to: " + String(static_cast<int>(SystemState::get().getMode())));
    programStartTemperature = getCurrentTemp();
    programStartTime = millis();
    startTimeOffset = 0;
    // curveManager = sourceCurveManager->clone();
    const Curve &curve = curveManager->getOriginalCurve();
    initialSegment = true;
    if (curve.elems[0].hTime == 0)
    {
        abort("No segments in curve");
        return;
    }
    // currentSegmentIndex =  determineStartSegment(curve, getCurrentTemp());
    curveManager->setSegmentIndex(determineStartSegment());
    //  Serial.println("__segIndex: " + String(curveManager->getSegmentIndex()));
    ratio = 0.18f;
    integral = 0;
    lastError = 0;
    heating->setCycleTime(SettingsManager::get().getSettings().heatingCycleMs);
    // heating->setEnabled(true);
    heating->setRatio(ratio);
    ResumeManager::saveCurveIndex(curveManager->getcurrentCurveIndex());
    // Serial.println("Starting process with curve index: " + String(curveManager->getcurrentCurveIndex()));
    useSegment();
    MeasurementManager::get().clear();
    

    // Serial.println("Process started with segment index: " + String(curveManager->getSegmentIndex()));
}

void ProcessController::checkSegmentAdvance()
{

    // const auto &segment = curveManager->getOriginalCurve().elems[curveManager->getSegmentIndex()];
    //  Serial.println("Checking segment advance: " + String(curveManager.getSegmentIndex()) + " " + String(segment.hTime) + " " + String(segment.endTemp));
    float currentTemp = getCurrentTemp();
    float targetTemp = curveManager->getSegmentTemp();
    bool skipUp = false;
    bool skipDown = false;
    if (curveManager->isSkip())
    {
        if (curveManager->getSegmentIndex() > 0 && (curveManager->getOriginalCurve().elems[curveManager->getSegmentIndex()].endTemp < curveManager->getOriginalCurve().elems[curveManager->getSegmentIndex() - 1].endTemp))
        {
            skipDown = true;
            // Serial.println("KURWA: " + String( curveManager->getOriginalCurve().elems[curveManager->getSegmentIndex()].endTemp < curveManager->getOriginalCurve().elems[curveManager->getSegmentIndex() - 1].endTemp) + " "+ String(curveManager->getOriginalCurve().elems[curveManager->getSegmentIndex()].endTemp) + "  " +String(curveManager->getOriginalCurve().elems[curveManager->getSegmentIndex() - 1].endTemp));
        }
        else
        {
            skipUp = true;
        }
    }

    bool reachedTempIncreasing = (segmentLine.a > epsilon || skipUp) && currentTemp >= targetTemp;
    bool reachedTempDecreasing = (segmentLine.a < -epsilon || (skipDown)) && currentTemp <= targetTemp;
    bool flatSegmentTimeOver = (std::abs(segmentLine.a) <= epsilon) && (millis() >= segmentEndTime);
    //  Serial.println("epsilon: " + String(epsilon) + " segmentLine.a: " + String(segmentLine.a) + " currentTemp: " + String(currentTemp) + " targetTemp: " + String(targetTemp) + " skip: " + String(skip));
    // Serial.println("Checking segment advance: " + String(curveManager.getSegmentIndex()) + " " + String(currentTemp) + " " + String(targetTemp) + " rTi " + String(reachedTempIncreasing) + " rTd " + String(reachedTempDecreasing) + " skipReached " + String(flatSegmentTimeOver));
    if (reachedTempIncreasing || reachedTempDecreasing || flatSegmentTimeOver)
    {
        Serial.println(String(reachedTempIncreasing) + " " + String(reachedTempDecreasing) + " " + String(flatSegmentTimeOver) + " " + String(targetTemp) + " " + String(skipUp) + " " + String(skipDown));
        nextSegment();
    }
}

void ProcessController::useSegment()
{
    MeasurementManager::get().addMeasurement();
    Serial.println("Using segment: " + String(curveManager->getSegmentIndex()) + " " + String(segmentEndTime));
    // const auto &curve = curveManager->getOriginalCurve();
    // const auto &segment = curve.elems[curveManager->getSegmentIndex()];
    initialSkipTime = 0;
    segmentStartTime = millis();
    segmentEndTime = segmentStartTime + curveManager->getSegmentTime();

    startTemp = curveManager->getSegmentStartTemperature();
    if (curveManager->isSkip())
    {
        maxSkipTime = 0;
        maxSkipTemp = 0;
        return;
    }
    segmentLine = Line(segmentStartTime, startTemp, segmentEndTime, curveManager->getSegmentTemp());
    if (initialSegment && abs(segmentLine.a) > epsilon)
    {
        unsigned long remainingTime = (segmentEndTime - segmentLine.x(getCurrentTemp()));
        segmentEndTime = remainingTime + millis();
        startTemp = getCurrentTemp();
        // curveManager->updateAdjustedCurve(curveManager->getSegmentIndex(),remainingTime);
        segmentLine = Line(segmentStartTime, startTemp, segmentEndTime, curveManager->getSegmentTemp());
        startTimeOffset += curveManager->getSegmentTime() - remainingTime;
        //  Serial.println(" use segment " + String(startTimeOffset) + " remainT " + String(remainingTime));
    }
}

void ProcessController::nextSegment()
{

    float lastA = segmentLine.a;
    // Serial.println("current segment end time: " + String(curveManager->getAdjustedCurve().elems[curveManager->getSegmentIndex()].hTime) + " current time: " + String(millis()) + " segment index: " + String(curveManager->getSegmentIndex()) + " lastA: " + String(lastA) );
    if (!initialSegment)
        curveManager->updateAdjustedCurve(curveManager->getSegmentIndex(), millis() - segmentStartTime);
    // Serial.println("current segment end time: " + String(curveManager->getAdjustedCurve().elems[curveManager->getSegmentIndex()].hTime) + " current time: " + String(millis()) + " segment index: " + String(curveManager->getSegmentIndex()) + " lastA: " + String(lastA) );
    // const Curve& orig = curveManager->getOriginalCurve();
    initialSegment = false;
    if (!curveManager->hasNextSegment())
    {
        finishFiring();
        return;
    }

    if (getMaxTemp(curveManager->getOriginalCurve()) == curveManager->getOriginalCurve().elems[curveManager->getSegmentIndex()].endTemp)
    {
        ResumeManager::clear();
    }

    // currentSegmentIndex++;
    curveManager->nextSegment();
    useSegment();
    if (lastA > segmentLine.a )
        ratio = ratio * getCurrentTemp()/1300.0f;
    SoundManager::beep(1000, 100);
}

uint8_t ProcessController::determineStartSegment()
{
    startTimeOffset = 0;
    // uint8_t elemWithCurrentTemp = 0;
    for (int i = 0; i < curveElemsNo; i++)
    {
        if (curveManager->getOriginalCurve().elems[i].endTemp > getCurrentTemp())
        {
            return i;
            break;
        }
        else
        {
            startTimeOffset += curveManager->getAdjustedCurve().elems[i].hTime;
            //  Serial.println("DetSS: sTO " + String(startTimeOffset));
        }
        if (curveManager->getOriginalCurve().elems[i].hTime == 0)
        {
            abort("start temp. too high");
        }
    }
    abort("cant determine start temp");
    return -1;
}

void ProcessController::applyPID()
{

    if (millis() - lastPidCheckTime < SettingsManager::get().getSettings().pidIntervalMs)
        return;
    if (SystemState::get().getMode() != SystemMode::Firing)
        return;
    if (curveManager->isSkip())
    {
        // Serial.println("Segment time is 0, skipping PID application.");
        if (curveManager->getSegmentTemp() < getCurrentTemp())
        {
            setHeaterPower(0);
        }
        else
        {
            setHeaterPower(1);
        }
        lastError = 0;
        return;
    }
    // Serial.println("2");
    lastPidCheckTime = millis();
    // Serial.println("3 " + String(segmentLine.a) + " " + String(segmentLine.b) + " " );
    float setpoint = segmentLine.y(millis());
    // Serial.println("4 " + String(setpoint));
    float currentTemp = getCurrentTemp();
    // Serial.println("5 " +  String(currentTemp));
    float error = setpoint - currentTemp;
    // Serial.println("6 " + String(error));
    if ((lastError > 0 && error < 0) || (lastError < 0 && error > 0))
    {
        integral = 0;
    }

    integral += error * (SettingsManager::get().getSettings().pidIntervalMs / 100.0f);
    integ = constrain((SettingsManager::get().getSettings().pid_ki / 10000.0f) * integral, -1, 1);
    deriv = SettingsManager::get().getSettings().pid_kd * (error - lastError) / (SettingsManager::get().getSettings().pidIntervalMs / 100.0f);
    prop = SettingsManager::get().getSettings().pid_kp * error / 1000.0f;
    // Serial.println("PID: " + String(prop) + " " + String(deriv) + " " + String(integ) + "; " + String(error) + " " + String(currentTemp) + " " + String(setpoint) + " " + String(ratio) + " " + String(integral) + " " + String(lastError) + " " + String(segmentLine.a, 6)  );
    // Serial.println(String(SettingsManager::get().getSettings().pid_kp) + " " + String(SettingsManager::get().getSettings().pid_ki) + " " + String(SettingsManager::get().getSettings().pid_kd) + " " + String(SettingsManager::get().getSettings().pidIntervalMs));
    float correction = prop + deriv + integ;
    ratio = constrain(ratio, 0, 1) + correction;
    lastError = error;

    setHeaterPower(ratio);
}

void ProcessController::setHeaterPower(float ratio)
{

    heating->setRatio(ratio);
    // furnace.setHeatingPower(ratio);
}
void ProcessController::finishFiring()
{
    MeasurementManager::get().addMeasurement();
    // heating->setEnabled(false);
    heating->stopHeating();
    SystemState::get().setMode(SystemMode::Idle);
    SoundManager::playFanfare();
    ResumeManager::clear();
    if (onError)
        onError("finished successfully");
}

void ProcessController::abort(const char *reason)
{
    MeasurementManager::get().addMeasurement();
    // heating->setEnabled(false);
    heating->stopHeating();
    String errorMessage = reason ? String(reason) : "Aborted";
    StorageManager::appendErrorLog(errorMessage);
    SystemState::get().setMode(SystemMode::Idle);
    SoundManager::beep(700, 400);
    ResumeManager::clear();
    if (onError)
        onError(reason);
    Serial.println("Process aborted: " + errorMessage);
}
float ProcessController::getMaxTemp(Curve c)
{
    long maxT = 0;
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
void ProcessController::checkForErrors()
{
    if (abs(lastError) > 100 && curveManager->getOriginalCurve().elems[curveManager->getSegmentIndex()].hTime > 60000)
    {
        abort(("Temp dev:" + String(curveManager->getOriginalCurve().elems[curveManager->getSegmentIndex()].hTime)).c_str());
    }
    else if (temperatureSensor->getCJTemperature() > 70)
    {
        abort("Controller box overheating");
    }
    else if (getCurrentTemp() > 1285)
    {
        abort("Temperature too high");
    }
    else if (temperatureSensor->getErrorCount() > TemperatureSensor::MAX_ERRORS)
    {
        abort("TC error count exceeded");
    }
    else if (IsHeatingStuckDuringSkipMode())
    {
        abort("Stuck during skip mode");
    }
}
float ProcessController::getCJTemp()
{
    return temperatureSensor->getCJTemperature();
}
float ProcessController::getCurrentTemp()
{
    return temperatureSensor->getTemperature();
}
bool ProcessController::IsHeatingStuckDuringSkipMode()
{
    if (curveManager->getSegmentTime() > 60000)
    {
        return false; // Nie jesteśmy w trybie skip, więc nie ma problemu
    }

    if (temperatureSensor->getTemperature() > maxSkipTemp)
    {
        maxSkipTemp = temperatureSensor->getTemperature(); // Zapisz nowe maksimum temperatury
        maxSkipTime = millis();                            // Zresetuj timer
    }

    // Czy minęło 200 sekund (3 min 20 s) bez wzrostu temperatury?
    return ((millis() - maxSkipTime) > 200000);
}
void ProcessController::adjustSkipTime()
{
    if (!curveManager->isSkip())
        return;
    if (!initialSegment)
    {
        curveManager->adjustSkipTime(getCurrentTemp() - curveManager->getSegmentStartTemperature(), millis() - segmentStartTime);
    }
    else
    {
        curveManager->adjustSkipTime(getCurrentTemp() - programStartTemperature, millis() - segmentStartTime);
        initialSkipTime = (millis() - segmentStartTime / getCurrentTemp() - programStartTemperature) * curveManager->getSegmentStartTemperature();
    }
}