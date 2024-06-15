#ifndef FRAMEREFRESH_H
#define FRAMEREFRESH_H

#include "VFDdriver.h"


namespace FrameRefresh {

  struct TimePeriod {
    int startHour;
    int startMinute;
    int endHour;
    int endMinute;
  };

  extern int last_number[6];
  extern int now_number[6];
  extern unsigned char dispalyTemp[6][5];
  extern unsigned char number[10][5];

  void up_now_num(String format_time);
  void up_last_num();
  void FrameRefresh(unsigned char num, unsigned char mCnt1, unsigned char mCnt2, unsigned char x);
  void SecDispalyRefresh();
  void FrameDisplayTime();
  bool display_time(const TimePeriod displayPeriods[], int numPeriods);
}

#endif
