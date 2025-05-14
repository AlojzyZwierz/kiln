#include "ProcessController.h"

extern FakeFurnace furnace;

void ProcessController::begin(CurveManager& cm, TemperatureSensor& sensor, MeasurementManager& mm) {
    sourceCurveManager = &cm;
    temperatureSensor = &sensor;
    measurementManager = &mm;
}

void ProcessController::startFiring() {  
    
    if (running) return; // Jeśli już działa, nie rób nic   
    curveManager = sourceCurveManager->clone();
    const Curve& curve = curveManager.getOriginalCurve(); 

    if (curve.elems[0].hTime == 0) {
        abort("No segments in curve");
        return;
    }   
    //currentSegmentIndex =  determineStartSegment(curve, getCurrentTemp());    
    curveManager.setSegmentIndex(determineStartSegment(curve, getCurrentTemp()));
    ratio = 0.07;
    integral = 0;
    running = true;
    curveStartTime = millis();
    heating.setCycleTime(SettingsManager::get().getSettings().heatingCycleMs);
    heating.setEnabled(true);
    heating.setRatio(ratio);
    ResumeManager::saveCurveIndex(curveManager.getcurrentCurveIndex());
    Serial.println("Starting process with curve index: " + String(curveManager.getcurrentCurveIndex()));
    useSegment();
    measurementManager->clear();
}

bool ProcessController::isRunning() const {
    return running;
}

void ProcessController::checkSegmentAdvance() {
    
    const auto& segment = curveManager.getOriginalCurve().elems[curveManager.getSegmentIndex()];
    
    float currentTemp = getCurrentTemp();
   
    float targetTemp = segment.endTemp;
    
    bool skip = segment.hTime == 60000;
    
    if (((segmentLine.a > epsilon)||skip  && currentTemp >= targetTemp) ||
        ((segmentLine.a < -epsilon)||skip && currentTemp <= targetTemp) ||
        (std::abs(segmentLine.a) <= epsilon && millis() >= segmentEndTime)) {
            
        nextSegment();
    }
    
}

void ProcessController::useSegment() {
    const auto& curve = curveManager.getOriginalCurve();
    const auto& segment = curve.elems[curveManager.getSegmentIndex()];

    segmentStartTime = millis();
    segmentEndTime = segmentStartTime + segment.hTime;

    if (curveManager.getSegmentIndex() == 0) {
        startTemp = getCurrentTemp();
    } else {
        startTemp = curve.elems[curveManager.getSegmentIndex() - 1].endTemp;
    }

    segmentLine = Line(segmentStartTime, startTemp, segmentEndTime, segment.endTemp);
    if (abs(segmentLine.a) > epsilon) {
        unsigned long remainingTime = (segmentEndTime - segmentLine.x(getCurrentTemp())) ;
        segmentEndTime = remainingTime + millis();
        segmentLine = Line(segmentStartTime, startTemp, segmentEndTime, segment.endTemp);
      }
}

void ProcessController::nextSegment() {
    
    float lastA = segmentLine.a;
    
    curveManager.updateAdjustedCurve(curveManager.getSegmentIndex(), millis() - segmentStartTime);

    const Curve& orig = curveManager.getOriginalCurve();

    if (curveManager.getSegmentIndex() + 1 >= curveElemsNo || orig.elems[curveManager.getSegmentIndex() + 1].hTime == 0) {
        finishFiring();
        return;
    }
    
    if (getMaxTemp(curveManager.getOriginalCurve()) == curveManager.getOriginalCurve().elems[curveManager.getSegmentIndex()].endTemp) {
        ResumeManager::clear();
      }
      
    //currentSegmentIndex++;
    curveManager.setSegmentIndex( curveManager.getSegmentIndex() + 1);
    useSegment();
    if(lastA > segmentLine.a) ratio=0;
    SoundManager::beep(1000, 100);

}

uint8_t ProcessController::determineStartSegment(const Curve& curve, float currentTemp) {
    for (int i = 0; i < curveElemsNo; ++i) {
        if (curve.elems[i].hTime == 0) break;
        if (curve.elems[i].endTemp > currentTemp) return i;
    }
    return 0;
}

void ProcessController::finishFiring() {
    running = false;
    heating.setEnabled(false);
    setHeaterPower(0);
    SoundManager::playFanfare();
}

float ProcessController::getCurrentTemp() {
    // TODO: Replace with actual temp reading
    //return temperatureSensor->getTemperature();
    return furnace.getTemperature();
}

void ProcessController::applyPID() {
    
    if (millis() - lastPidCheckTime < SettingsManager::get().getSettings().pidIntervalMs) return;
    if (!running) return;
    
    //Serial.println("2");
    lastPidCheckTime = millis();
    //Serial.println("3 " + String(segmentLine.a) + " " + String(segmentLine.b) + " " );
    float setpoint = segmentLine.y(millis());
    //Serial.println("4 " + String(setpoint));
    float currentTemp = getCurrentTemp();
    //Serial.println("5 " +  String(currentTemp));
    float error = setpoint - currentTemp;
//Serial.println("6 " + String(error));
    if ((lastError > 0 && error < 0) || (lastError < 0 && error > 0)) {
        integral = 0;
    }

    integral += error * (SettingsManager::get().getSettings().pidIntervalMs / 100.0f);
    float integ = constrain((SettingsManager::get().getSettings().pid_ki / 10000.0f) * integral, -1, 1);
    float deriv = SettingsManager::get().getSettings().pid_kd * (error - lastError) / (SettingsManager::get().getSettings().pidIntervalMs / 100.0f);
    float prop = SettingsManager::get().getSettings().pid_kp * error / 1000.0f;
Serial.println("PID: " + String(prop) + " " + String(deriv) + " " + String(integ) + " " + String(error) + " " + String(currentTemp) + " " + String(setpoint) + " " + String(ratio) + " " + String(integral) + " " + String(lastError) + " " + String(segmentLine.a)  );
    Serial.println(String(SettingsManager::get().getSettings().pid_kp) + " " + String(SettingsManager::get().getSettings().pid_ki) + " " + String(SettingsManager::get().getSettings().pid_kd) + " " + String(SettingsManager::get().getSettings().pidIntervalMs));
    float correction = prop + deriv + integ;
    ratio = constrain(ratio , 0, 1)+ correction;
    lastError = error;

    setHeaterPower(ratio);
}

void ProcessController::setHeaterPower(float ratio) {
    
    //heating.setRatio(ratio);
    furnace.setHeatingPower(ratio);
}

void ProcessController::abort(const char* reason) {
    running = false;
    //digitalWrite(SSR, LOW);
    heating.setEnabled(false);


    // Można wyczyścić inne rzeczy, np. pomiary, PID, itp.
    String errorMessage = reason ? String(reason) : "Aborted";

    // Zapisz informację o awarii, np. do konfiguracji

    StorageManager::appendErrorLog(errorMessage);
    // Ewentualnie sygnał dźwiękowy
    //tone(BUZZERPIN, 1000, 500);
    SoundManager::beep(1000, 500);
}
 float ProcessController::getMaxTemp(Curve c){
    long maxT = 0;
    for (int i = 0; i < curveElemsNo; i++) {
      if (c.elems[i].hTime == 0) break;
      if (c.elems[i].endTemp > maxT) {
        maxT = c.elems[i].endTemp;
      }
    }
    return maxT;
  }

