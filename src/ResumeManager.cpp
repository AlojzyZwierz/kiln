#include "ResumeManager.h"
const char *resumePath = "/resume.dat";
static bool readData(ResumeData &d)
{
    if (!SPIFFS.exists(resumePath))
        return false;
    File f = SPIFFS.open(resumePath, "r");
    if (!f)
        return false;
    bool ok = f.size() == sizeof(ResumeData) &&
              f.readBytes((char *)&d, sizeof(d)) == sizeof(d) &&
              d.magic == 0xA5 && d.curve >= 0 &&
              (d.type == ResumeType::Firing || d.type == ResumeType::Armed);
    f.close();
    return ok;
}

static void writeData(const ResumeData &d)
{
    File f = SPIFFS.open(resumePath, "w"); // "w" sam obcina plik, remove() niepotrzebne
    if (!f)
        return;
    f.write((const uint8_t *)&d, sizeof(d));
    f.close();
}

bool ResumeManager::hasResumeData()
{
    ResumeData d;
    return readData(d) && d.type == ResumeType::Firing;
}
int ResumeManager::loadCurveIndex()
{
    ResumeData d;
    return (readData(d) && d.type == ResumeType::Firing) ? d.curve : -1;
}
void ResumeManager::saveCurveIndex(int i) { writeData({0xA5, ResumeType::Firing, (int16_t)i, 0}); }

void ResumeManager::saveArmed(int curve, uint32_t remainingMs) { writeData({0xA5, ResumeType::Armed, (int16_t)curve, remainingMs}); }
bool ResumeManager::loadArmed(int &curve, uint32_t &remainingMs)
{
    ResumeData d;
    if (!readData(d) || d.type != ResumeType::Armed)
        return false;
    curve = d.curve;
    remainingMs = d.remainingMs;
    return true;
}
void ResumeManager::clearArmed()
{
    ResumeData d;
    if (readData(d) && d.type == ResumeType::Armed)
        clear();
}

void ResumeManager::clear()
{
    if (SPIFFS.remove(resumePath))
    {
        Serial.println("Resume data cleared");
    }
    else
    {
        Serial.println("Failed to clear resume data");
    }
}