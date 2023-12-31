#include "time.h"
#include "sntp.h"
#include "sys/app_controller.h"

class SystemTime
{
private:
    const char *ntpServer1 = "ntp.aliyun.com";
    const char *ntpServer2 = "ntp2.aliyun.com";
    const char *ntpServer3 = "time.nist.gov";
    long timezone = +8;

public:
    SystemTime()
    {
        configTime(timezone * 3600, 0, ntpServer1, ntpServer2, ntpServer3);
        setTime(0, 0, 12, 13, 12, 2023);
    }

    ~SystemTime() {}

    tm time()
    {
        struct tm timeinfo;

        if (!getLocalTime(&timeinfo))
        {
            Serial.println("Failed to obtain time");
        }
        else
        {
            Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
        }
        return timeinfo;
    }

    void setTime(int sc, int mn, int hr, int dy, int mt, int yr)
    {
        struct tm t = {0};     // Initalize to all 0's
        t.tm_year = yr - 1900; // This is year-1900, so 121 = 2021
        t.tm_mon = mt - 1;
        t.tm_mday = dy;
        t.tm_hour = hr;
        t.tm_min = mn;
        t.tm_sec = sc;
        time_t timeSinceEpoch = mktime(&t);
        struct timeval now = {.tv_sec = timeSinceEpoch};
        settimeofday(&now, NULL);
    }

    // Single instance
    static SystemTime &instance()
    {
        static SystemTime instance;
        return instance;
    }
};
