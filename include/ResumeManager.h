#pragma once
#include <FS.h>
#include <SPIFFS.h>
enum class ResumeType : uint8_t
{
    Firing = 1,
    Armed = 2
};

struct ResumeData
{
    uint8_t magic; // 0xA5
    ResumeType type;
    int16_t curve;
    uint32_t remainingMs; // tylko Armed
};

class ResumeManager

{
public:
    static bool hasResumeData();
    static int loadCurveIndex();
    static void saveCurveIndex(int index);
    static void clear();
    static void saveArmed(int curve, uint32_t remainingMs);
    static bool loadArmed(int &curve, uint32_t &remainingMs);
    static void clearArmed();
};
