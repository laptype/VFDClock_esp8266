#ifndef FRAMEREFRESH_H
#define FRAMEREFRESH_H

#include "VFDdriver.h"
#include <TimeLib.h>

class FrameRefresh {
public:
    struct TimePeriod {
      int startHour;
      int startMinute;
      int endHour;
      int endMinute;
    };
    FrameRefresh();
    void setFont(bool bold);
    void FrameDisplayTime();
    bool display_time(const TimePeriod displayPeriods[], int numPeriods);
    void freshDisplay();
private:
    void up_now_num(String format_time);
    void up_last_num();
    void FrameRefreshFunc(unsigned char num, unsigned char mCnt1, unsigned char mCnt2, unsigned char x);
    void disPlayFont(int num, unsigned char x);
    void SecDispalyRefresh();
    bool isWithinTimePeriod(int hour, int minute, const TimePeriod& period);
    void init_last_num();
    int last_number[6] = {11, 11, 11, 11, 11, 11};
    int now_number[6] = {0, 0, 0, 0, 0, 0};
    unsigned char dispalyTemp[6][5];
    bool isBold;
    unsigned char (*currentNumber)[5];

    static unsigned char number[12][5];
    static unsigned char numberBold[12][5];
};

#endif
