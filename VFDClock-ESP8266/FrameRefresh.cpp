#include "FrameRefresh.h"
unsigned char FrameRefresh::numberFontNormal[12][5] = {
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

unsigned char FrameRefresh::numberFontBold[12][5] = {
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

FrameRefresh::FrameRefresh() : currentNumberFont(numberFontNormal) {}

void FrameRefresh::setFont(bool bold) {
    if (bold) {
        currentNumberFont = numberFontBold;
    } else {
        currentNumberFont = numberFontNormal;
    }
    freshDisplay();
}

void NumberArray::update(const char* timeBuffer) {
    numbers[0] = timeBuffer[0] - '0';
    numbers[1] = timeBuffer[1] - '0';
    numbers[2] = timeBuffer[3] - '0';
    numbers[3] = timeBuffer[4] - '0';
    numbers[4] = timeBuffer[6] - '0';
    numbers[5] = timeBuffer[7] - '0';
}

void NumberArray::update(int countdownHour, int countdownMinute, int countdownSecond) {
    numbers[0] = countdownHour / 10;
    numbers[1] = countdownHour % 10;
    numbers[2] = countdownMinute / 10;
    numbers[3] = countdownMinute % 10;
    numbers[4] = countdownSecond / 10;
    numbers[5] = countdownSecond % 10;
}

NumberArray& NumberArray::operator=(const NumberArray& other) {
    if (this != &other) {
        for (size_t i = 0; i < 6; ++i) {
            numbers[i] = other.numbers[i];
        }
    }
    return *this;
}

int& NumberArray::operator[](size_t index) {
    if (index >= 6) {
        return numbers[0];
    }
    return numbers[index];
}

const int& NumberArray::operator[](size_t index) const {
    if (index >= 6) {
        return numbers[0];
    }
    return numbers[index];
}

void FrameRefresh::FrameRefreshFunc(unsigned char num, unsigned char mCnt1, unsigned char mCnt2, unsigned char x) {
    unsigned char tmpDisplay[5];
    tmpDisplay[0] = (currentNumberFont[pre_number[num]][0] << mCnt1) | (currentNumberFont[cur_number[num]][0] >> mCnt2);
    tmpDisplay[1] = (currentNumberFont[pre_number[num]][1] << mCnt1) | (currentNumberFont[cur_number[num]][1] >> mCnt2);
    tmpDisplay[2] = (currentNumberFont[pre_number[num]][2] << mCnt1) | (currentNumberFont[cur_number[num]][2] >> mCnt2);
    tmpDisplay[3] = (currentNumberFont[pre_number[num]][3] << mCnt1) | (currentNumberFont[cur_number[num]][3] >> mCnt2);
    tmpDisplay[4] = (currentNumberFont[pre_number[num]][4] << mCnt1) | (currentNumberFont[cur_number[num]][4] >> mCnt2);
    VFD_WriteUserFont(x, x, tmpDisplay);
}

void FrameRefresh::disPlayFont(int num, unsigned char x) {
    VFD_WriteUserFont(x, x, currentNumberFont[num]);
}

void FrameRefresh::freshDisplay() {
  disPlayFont(cur_number[0], 0);
  disPlayFont(cur_number[1], 1);
  disPlayFont(10, 2);
  disPlayFont(cur_number[2], 3);
  disPlayFont(cur_number[3], 4);
  disPlayFont(10, 5);
  disPlayFont(cur_number[4], 6);
  disPlayFont(cur_number[5], 7);
  delay(10);
}

void FrameRefresh::reset_number() {
    cur_number.update(0, 0, 0);
    pre_number.update(0, 0, 0);
}

void FrameRefresh::SecDisplayRefresh(int delayTime) {
    for (unsigned char i = 1; i <= 7; ++i) {
        if (cur_number[0] != pre_number[0]) FrameRefreshFunc(0, i, 7 - i, 0);
        if (cur_number[1] != pre_number[1]) FrameRefreshFunc(1, i, 7 - i, 1);
        if (cur_number[2] != pre_number[2]) FrameRefreshFunc(2, i, 7 - i, 3);
        if (cur_number[3] != pre_number[3]) FrameRefreshFunc(3, i, 7 - i, 4);
        if (cur_number[4] != pre_number[4]) FrameRefreshFunc(4, i, 7 - i, 6);
        if (cur_number[5] != pre_number[5]) FrameRefreshFunc(5, i, 7 - i, 7);
        delay(delayTime);
    }
}

void FrameRefresh::displayFrameTime(int currentHour, int currentMinute, int currentSecond) {
    // char timeBuffer[9];
    // sprintf(timeBuffer, "%02d:%02d:%02d", currentHour, currentMinute, currentSecond);
    cur_number.update(currentHour, currentMinute, currentSecond);
    SecDisplayRefresh(15);
    pre_number = cur_number;
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

bool FrameRefresh::isDisplay(const TimePeriod displayPeriods[], int numPeriods) {
    int currentHour = hour();
    int currentMinute = minute();

    for (int i = 0; i < numPeriods; ++i) {
        if (isWithinTimePeriod(currentHour, currentMinute, displayPeriods[i])) {
            return true;
        }
    }
    return false;    
}

void FrameRefresh::enableDisplay(bool enable) {
  if (enable) {
    VFD_enable(true);
    this->freshDisplay();
    delay(20);
    this->freshDisplay();
  } else {
    VFD_enable(false);    
  }
}

void FrameRefresh::setBrightness(int brightness) {
    VFD_setBrightness(brightness);
}