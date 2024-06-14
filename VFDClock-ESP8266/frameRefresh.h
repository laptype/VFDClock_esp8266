#ifndef FRAMEREFRESH_H
#define FRAMEREFRESH_H
#include "VFDdriver.h"

int last_number[6] = {0,0,0,0,0,0};
int now_number[6] = {6,6,6,6,6,6};
unsigned char dispalyTemp[6][5] = {0};

unsigned char number[10][5] = {
   0x3E, 0x51, 0x49, 0x45, 0x3E,// 0
   0x00, 0x42, 0x7F, 0x40, 0x00,// 1
   0x42, 0x61, 0x51, 0x49, 0x46,// 2
   0x21, 0x41, 0x45, 0x4B, 0x31,// 3
   0x18, 0x14, 0x12, 0x7F, 0x10,// 4
   0x27, 0x45, 0x45, 0x45, 0x39,// 5
   0x3C, 0x4A, 0x49, 0x49, 0x30,// 6
   0x01, 0x71, 0x09, 0x05, 0x03,// 7
   0x36, 0x49, 0x49, 0x49, 0x36,// 8
   0x06, 0x49, 0x49, 0x29, 0x1E,// 9
};


void up_now_num(String format_time)
{
  unsigned char i;
  now_number[0] = format_time[0]-'0';
  now_number[1] = format_time[1]-'0';
  now_number[2] = format_time[3]-'0';
  now_number[3] = format_time[4]-'0';
  now_number[4] = format_time[6]-'0';
  now_number[5] = format_time[7]-'0';
}
void up_last_num(){
  last_number[0] = now_number[0];
  last_number[1] = now_number[1];
  last_number[2] = now_number[2];
  last_number[3] = now_number[3];
  last_number[4] = now_number[4];
  last_number[5] = now_number[5];
}

void FrameRefresh(unsigned char num, unsigned char mCnt1, unsigned char mCnt2, unsigned char x)
{
  dispalyTemp[num][0] = (number[last_number[num]][0] << mCnt1) | (number[now_number[num]][0] >> mCnt2);
  dispalyTemp[num][1] = (number[last_number[num]][1] << mCnt1) | (number[now_number[num]][1] >> mCnt2);  
  dispalyTemp[num][2] = (number[last_number[num]][2] << mCnt1) | (number[now_number[num]][2] >> mCnt2);
  dispalyTemp[num][3] = (number[last_number[num]][3] << mCnt1) | (number[now_number[num]][3] >> mCnt2);
  dispalyTemp[num][4] = (number[last_number[num]][4] << mCnt1) | (number[now_number[num]][4] >> mCnt2);
  dispalyTemp[num][5] = (number[last_number[num]][5] << mCnt1) | (number[now_number[num]][5] >> mCnt2);
  VFD_WriteUserFont(x,x,dispalyTemp[num]);
}

void SecDispalyRefresh() {
  unsigned char i;
  for(i=1;i<7;i++) {

    if (now_number[0] != last_number[0]) FrameRefresh(0,i,7-i,0);
    if (now_number[1] != last_number[1]) FrameRefresh(1,i,7-i,1);
    if (now_number[2] != last_number[2]) FrameRefresh(2,i,7-i,3);
    if (now_number[3] != last_number[3]) FrameRefresh(3,i,7-i,4);
    if (now_number[4] != last_number[4]) FrameRefresh(4,i,7-i,6);
    if (now_number[5] != last_number[5]) FrameRefresh(5,i,7-i,7);

    delay(20);
  }
}



#endif