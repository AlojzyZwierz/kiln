#include "Utils.h"

long Utils::HMStoMillis(int h, int m, int s)
{
    return (h * 3600000L + m * 60000L + s * 1000L);
}

String Utils::millisToHMS(long t)
{
    String toReturn = "";
    if (t < 0)
    {
        toReturn = "-";
        t = -t;
    }
    long h = t / 3600000;
    long m = (t % 3600000) / 60000;
    long s = (t % 60000) / 1000;
    return toReturn + leadingZero(h) + ":" + leadingZero(m) + ":" + leadingZero(s);
}

String Utils::millisToHM(long t)
{
    String toReturn = "";
    if (t < 0)
    {
        toReturn = "-";
        t = -t;
    }
    long h = t / 3600000;
    long m = (t % 3600000) / 60000;
    return toReturn + leadingZero(h) + ":" + leadingZero(m);
}

String Utils::leadingZero(int num, int width)
{
    String s = String(num);
    while (s.length() < width)
        s = "0" + s;
    return s;
}
