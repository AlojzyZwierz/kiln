#include "ResumeManager.h"
const char *resumePath = "/resume.dat";


bool ResumeManager::hasResumeData()
{
    Serial.println("Checking for resume data at: " + String(resumePath));
    return SPIFFS.exists(resumePath);
}

int ResumeManager::loadCurveIndex()
{
    if(!hasResumeData()) {
        return -1;
    }

    File file = SPIFFS.open(resumePath, "r");
    if (!file || file.size() < sizeof(int)) {
        if(file) file.close();
        return -1;
    }

    int index = -1;
    if(file.readBytes((char *)&index, sizeof(index)) != sizeof(index)) {
        file.close();
        return -1;
    }
    
    file.close();
    Serial.println("Loaded curve index: " + String(index));
    return index;
}

void ResumeManager::saveCurveIndex(int index)
{
    File file = SPIFFS.open(resumePath, "w");
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }

    if(file.write((uint8_t *)&index, sizeof(index)) != sizeof(index)) {
        Serial.println("Failed to write data");
    }
    
    file.close();
    Serial.println("Saved curve index: " + String(index));
}

void ResumeManager::clear()
{
    if(SPIFFS.remove(resumePath)) {
        Serial.println("Resume data cleared");
    } else {
        Serial.println("Failed to clear resume data");
    }
}