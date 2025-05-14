#pragma once
#include <FS.h>
#include <SPIFFS.h>
class ResumeManager {
public:
    static bool hasResumeData();
    static int loadCurveIndex();
    static void saveCurveIndex(int index);
    static void clear();
};

