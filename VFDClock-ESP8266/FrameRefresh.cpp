#include "FrameRefresh.h"
unsigned char FrameRefresh::number[12][5] = {
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
    {0x62, 0x51, 0x49, 0x49, 0x46}, // 2
    {0x22, 0x49, 0x49, 0x49, 0x36}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3E, 0x49, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x49, 0x3E},  // 9
    {0x00, 0x00, 0x36, 0x00, 0x00},  // :
    {0x00, 0x00, 0x00, 0x00, 0x00}  // null
};

unsigned char FrameRefresh::numberBold[12][5] = {
    {0x7F, 0x7F, 0x41, 0x7F, 0x7F}, // 0
    {0x00, 0x7F, 0x7F, 0x7F, 0x00}, // 1
    {0x79, 0x79, 0x69, 0x6F, 0x6F}, // 2
    {0x6B, 0x6B, 0x6B, 0x7F, 0x7F}, // 3
    {0x1F, 0x1F, 0x18, 0x7F, 0x7F}, // 4
    {0x6F, 0x6F, 0x69, 0x79, 0x79}, // 5
    {0x7F, 0x7F, 0x49, 0x79, 0x79}, // 6
    {0x03, 0x03, 0x03, 0x7F, 0x7F}, // 7
    {0x7F, 0x7F, 0x49, 0x7F, 0x7F}, // 8
    {0x4F, 0x4F, 0x49, 0x7F, 0x7F},  // 9
    {0x00, 0x36, 0x36, 0x36, 0x00},  // :
    {0x00, 0x00, 0x00, 0x00, 0x00}  // null
};

FrameRefresh::FrameRefresh() : isBold(false), currentNumber(number) {
    for (int i = 0; i < 6; ++i) {
        last_number[i] = 11;
        now_number[i] = 6;
    }
}

void FrameRefresh::setFont(bool bold) {
    isBold = bold;
    if (isBold) {
        currentNumber = numberBold;
    } else {
        currentNumber = number;
    }
    freshDisplay();
}

void FrameRefresh::up_now_num(String format_time) {
    now_number[0] = format_time[0] - '0';
    now_number[1] = format_time[1] - '0';
    now_number[2] = format_time[3] - '0';
    now_number[3] = format_time[4] - '0';
    now_number[4] = format_time[6] - '0';
    now_number[5] = format_time[7] - '0';
}

void FrameRefresh::up_last_num() {
    for (int i = 0; i < 6; ++i) {
        last_number[i] = now_number[i];
    }
}

void FrameRefresh::init_last_num() {
    for (int i = 0; i < 6; ++i) {
        last_number[i] = 11;
    }
}

void FrameRefresh::FrameRefreshFunc(unsigned char num, unsigned char mCnt1, unsigned char mCnt2, unsigned char x) {
    dispalyTemp[num][0] = (currentNumber[last_number[num]][0] << mCnt1) | (currentNumber[now_number[num]][0] >> mCnt2);
    dispalyTemp[num][1] = (currentNumber[last_number[num]][1] << mCnt1) | (currentNumber[now_number[num]][1] >> mCnt2);
    dispalyTemp[num][2] = (currentNumber[last_number[num]][2] << mCnt1) | (currentNumber[now_number[num]][2] >> mCnt2);
    dispalyTemp[num][3] = (currentNumber[last_number[num]][3] << mCnt1) | (currentNumber[now_number[num]][3] >> mCnt2);
    dispalyTemp[num][4] = (currentNumber[last_number[num]][4] << mCnt1) | (currentNumber[now_number[num]][4] >> mCnt2);
    VFD_WriteUserFont(x, x, dispalyTemp[num]);
}

void FrameRefresh::disPlayFont(int num, unsigned char x) {
    VFD_WriteUserFont(x, x, currentNumber[num]);
}

void FrameRefresh::freshDisplay() {
  disPlayFont(now_number[0], 0);
  disPlayFont(now_number[1], 1);
  disPlayFont(10, 2);
  disPlayFont(now_number[2], 3);
  disPlayFont(now_number[3], 4);
  disPlayFont(10, 5);
  disPlayFont(now_number[4], 6);
  disPlayFont(now_number[5], 7);
  delay(10);
}

void FrameRefresh::SecDispalyRefresh() {
    for (unsigned char i = 1; i <= 7; ++i) {
        if (now_number[0] != last_number[0]) FrameRefreshFunc(0, i, 7 - i, 0);
        if (now_number[1] != last_number[1]) FrameRefreshFunc(1, i, 7 - i, 1);
        if (now_number[2] != last_number[2]) FrameRefreshFunc(2, i, 7 - i, 3);
        if (now_number[3] != last_number[3]) FrameRefreshFunc(3, i, 7 - i, 4);
        if (now_number[4] != last_number[4]) FrameRefreshFunc(4, i, 7 - i, 6);
        if (now_number[5] != last_number[5]) FrameRefreshFunc(5, i, 7 - i, 7);
        delay(15);
    }
}

void FrameRefresh::FrameDisplayTime() {
    int currentHour = hour();
    int currentMinute = minute();
    int currentSecond = second();
    char timeBuffer[9];
    sprintf(timeBuffer, "%02d:%02d:%02d", currentHour, currentMinute, currentSecond);

    String format_time = String(timeBuffer);
    up_now_num(format_time);
    SecDispalyRefresh();
    up_last_num();
    // VFD_WriteStr(0, timeBuffer);    
}

bool FrameRefresh::isWithinTimePeriod(int hour, int minute, const TimePeriod& period) {
    if (hour < period.startHour || hour > period.endHour) {
        return false;
    }
    if (hour == period.startHour && minute < period.startMinute) {
        return false;
    }
    if (hour == period.endHour && minute > period.endMinute) {
        return false;
    }
    return true;
}

bool FrameRefresh::display_time(const TimePeriod displayPeriods[], int numPeriods) {
    int currentHour = hour();
    int currentMinute = minute();

    for (int i = 0; i < numPeriods; ++i) {
        if (isWithinTimePeriod(currentHour, currentMinute, displayPeriods[i])) {
            return true;
        }
    }
    return false;    
}