#include "ResumeManager.h"
const char* resumePath = "/resume.dat";

bool ResumeManager::hasResumeData() {
    return SPIFFS.exists(resumePath);
}

int ResumeManager::loadCurveIndex() {
    File file = SPIFFS.open(resumePath, "r");
    if (!file) return -1;

    int index = -1;
    file.readBytes((char*)&index, sizeof(index));
    file.close();
    return index;
}

void ResumeManager::saveCurveIndex(int index) {
    File file = SPIFFS.open(resumePath, "w");
    if (!file) return;

    file.write((uint8_t*)&index, sizeof(index));
    file.close();
}

void ResumeManager::clear() {
    SPIFFS.remove(resumePath);
}
