#include "ProcessController.h"

// extern FakeFurnace furnace;

void ProcessController::begin(CurveManager &cm, TemperatureSensor &sensor, HeatingController &h)
{
    curveManager = &cm;
    temperatureSensor = &sensor;
    // measurementManager = &mm;
    heating = &h;
}

void ProcessController::startFiring()
{
    Serial.println("Starting firing process...");
    if (SystemState::get().getMode() == SystemMode::Firing)
        return; // Jeśli już działa, nie rób nic
    SystemState::get().setMode(SystemMode::Firing);
    Serial.println("System mode set to: " + String(static_cast<int>(SystemState::get().getMode())));
    programStartTemperature = getCurrentTemp();
    programStartTime = millis();
    startTimeOffset = 0;
    // curveManager = sourceCurveManager->clone();
    const Curve &curve = curveManager->getOriginalCurve();

    if (curve.elems[0].hTime == 0)
    {
        abort("No segments in curve");
        return;
    }
    // currentSegmentIndex =  determineStartSegment(curve, getCurrentTemp());
    curveManager->setSegmentIndex(determineStartSegment(curve, getCurrentTemp()));
    Serial.println("__segIndex: " + String(curveManager->getSegmentIndex()));
    ratio = 0.35f;
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

    const auto &segment = curveManager->getOriginalCurve().elems[curveManager->getSegmentIndex()];
    // Serial.println("Checking segment advance: " + String(curveManager.getSegmentIndex()) + " " + String(segment.hTime) + " " + String(segment.endTemp));
    float currentTemp = getCurrentTemp();
    float targetTemp = segment.endTemp;
    bool skip = segment.hTime == 60000;

    bool reachedTempIncreasing = (segmentLine.a > epsilon || skip) && currentTemp >= targetTemp;
    bool reachedTempDecreasing = (segmentLine.a < -epsilon || skip) && currentTemp <= targetTemp;
    bool flatSegmentTimeOver = (std::abs(segmentLine.a) <= epsilon) && (millis() >= segmentEndTime);
    //  Serial.println("epsilon: " + String(epsilon) + " segmentLine.a: " + String(segmentLine.a) + " currentTemp: " + String(currentTemp) + " targetTemp: " + String(targetTemp) + " skip: " + String(skip));
    // Serial.println("Checking segment advance: " + String(curveManager.getSegmentIndex()) + " " + String(currentTemp) + " " + String(targetTemp) + " rTi " + String(reachedTempIncreasing) + " rTd " + String(reachedTempDecreasing) + " skipReached " + String(flatSegmentTimeOver));
    if (reachedTempIncreasing || reachedTempDecreasing || flatSegmentTimeOver)
    {
        nextSegment();
    }
}

void ProcessController::useSegment()
{
    Serial.println("Using segment: " + String(curveManager->getSegmentIndex()));
    const auto &curve = curveManager->getOriginalCurve();
    const auto &segment = curve.elems[curveManager->getSegmentIndex()];

    segmentStartTime = millis();
    segmentEndTime = segmentStartTime + segment.hTime;

    if (curveManager->getSegmentIndex() == 0)
    {
        // startTemp = getCurrentTemp();
        startTemp = 20;
    }
    else
    {
        startTemp = curve.elems[curveManager->getSegmentIndex() - 1].endTemp;
    }

    segmentLine = Line(segmentStartTime, startTemp, segmentEndTime, segment.endTemp);
    if (abs(segmentLine.a) > epsilon)
    {
        unsigned long remainingTime = (segmentEndTime - segmentLine.x(getCurrentTemp()));
        segmentEndTime = remainingTime + millis();
        startTemp = getCurrentTemp();
        // curveManager->updateAdjustedCurve(curveManager->getSegmentIndex(),remainingTime);
        segmentLine = Line(segmentStartTime, startTemp, segmentEndTime, segment.endTemp);
        startTimeOffset += segmentEndTime - remainingTime;
    }
}

void ProcessController::nextSegment()
{

    float lastA = segmentLine.a;
    // Serial.println("current segment end time: " + String(curveManager->getAdjustedCurve().elems[curveManager->getSegmentIndex()].hTime) + " current time: " + String(millis()) + " segment index: " + String(curveManager->getSegmentIndex()) + " lastA: " + String(lastA) );
    curveManager->updateAdjustedCurve(curveManager->getSegmentIndex(), millis() - segmentStartTime);
    // Serial.println("current segment end time: " + String(curveManager->getAdjustedCurve().elems[curveManager->getSegmentIndex()].hTime) + " current time: " + String(millis()) + " segment index: " + String(curveManager->getSegmentIndex()) + " lastA: " + String(lastA) );
    // const Curve& orig = curveManager->getOriginalCurve();

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
    if (lastA > segmentLine.a)
        ratio = 0;
    SoundManager::beep(1000, 100);
}

uint8_t ProcessController::determineStartSegment(const Curve &curve, float currentTemp)
{
    startTimeOffset = 0;
    // uint8_t elemWithCurrentTemp = 0;
    for (int i = 0; i < curveElemsNo; i++)
    {
        if (curve.elems[i].endTemp > currentTemp)
        {
            return i;
            break;
        }
        if (curve.elems[i].hTime == 0)
        {
            abort("start temp. too high");
        }
        startTimeOffset += curve.elems[i].hTime;
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
    if (curveManager->getSegmentTime() == 60000)
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
    Serial.println(String(SettingsManager::get().getSettings().pid_kp) + " " + String(SettingsManager::get().getSettings().pid_ki) + " " + String(SettingsManager::get().getSettings().pid_kd) + " " + String(SettingsManager::get().getSettings().pidIntervalMs));
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
    // heating->setEnabled(false);
    setHeaterPower(0);
    SystemState::get().setMode(SystemMode::Idle);
    SoundManager::playFanfare();
    ResumeManager::clear();
    if (onError)
        onError("finished successfully");
}

void ProcessController::abort(const char *reason)
{
    // heating->setEnabled(false);
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
    if (abs(lastError) > 100 && curveManager->getSegmentTime() > 60000)
    {
        abort(("Temp dev:" + String(lastError)).c_str());
    }
    else if (temperatureSensor->getCJTemperature() > 70)
    {
        abort("controller box overheating");
    }
    else if (getCurrentTemp() > 1285)
    {
        abort("Kiln temperature too high");
    }
    else if (temperatureSensor->getErrorCount() > TemperatureSensor::MAX_ERRORS)
    {
        abort("Temperature sensor \n error count exceeded");
    }
    else if (IsHeatingStuckDuringSkipMode())
    {
        abort("Heating stuck during skip mode");
    }
}
float ProcessController::getCJTemp()
{
    return temperatureSensor->getCJTemperature();
}
float ProcessController::getCurrentTemp(){
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
    return (millis() - maxSkipTime > 200000);
}
