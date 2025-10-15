#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

class Utils
{
public:
    static long HMStoMillis(int h, int m, int s);
    static String millisToHMS(long t);
    static String millisToHM(long t);
    static String leadingZero(int num, int width = 2);
    static void printMemoryInfo();
};

#endif
