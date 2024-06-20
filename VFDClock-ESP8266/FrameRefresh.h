#ifndef FRAMEREFRESH_H
#define FRAMEREFRESH_H

#include "VFDdriver.h"
#include <TimeLib.h>

struct NumberArray {
    int numbers[6] = {0, 0, 0, 0, 0, 0};

    NumberArray(const String format_time) { update(format_time.c_str()); }
    NumberArray(int n) { for (int i = 0; i < 6; ++i) numbers[i] = n; }

    void update(const char* format_time);
    void update(int countdownHour, int countdownMinute, int countdownSecond);
    
    NumberArray& operator=(const NumberArray& other);
    int& operator[](size_t index);
    const int& operator[](size_t index) const;
};

class FrameRefresh {
public:
    struct TimePeriod {
      int startHour;
      int startMinute;
      int endHour;
      int endMinute;
    };
    FrameRefresh();
    void displayFrameTime(int currentHour, int currentMinute, int currentSecond);
    void freshDisplay();
    void enableDisplay(bool en);

    void setFont(bool bold);
    void setBrightness(int brightness);
    static bool isDisplay(const TimePeriod displayPeriods[], int numPeriods);
private:
    void FrameRefreshFunc(unsigned char num, unsigned char mCnt1, unsigned char mCnt2, unsigned char x);
    void disPlayFont(int num, unsigned char x);
    void SecDispalyRefresh();

    NumberArray pre_number = NumberArray(11);
    NumberArray cur_number = NumberArray(0);

    unsigned char (*currentNumberFont)[5];

    static unsigned char numberFontNormal[12][5];
    static unsigned char numberFontBold[12][5];
    static bool isWithinTimePeriod(int hour, int minute, const TimePeriod& period);
};

#endif
