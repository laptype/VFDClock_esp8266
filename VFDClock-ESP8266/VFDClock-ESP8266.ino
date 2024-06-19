#include "FrameRefresh.h"
#include "WebControl.h"
#include <TimeLib.h> // Include the Time library for time functions
// #include <ESP8266WebServer.h>
// ESP8266WebServer server(80);

FrameRefresh::TimePeriod displayPeriods[] = {
    {10, 0, 12, 0},
    {13, 0, 17, 30},
    {18, 0, 23, 0},
};

bool enable_display = true;

FrameRefresh frame = FrameRefresh();
WebControl webControl = WebControl(&frame); // 创建WebControl对象

int countdownHour=1;
int countdownMinute=1;
int countdownSecond=0;

void setup() {

  // Serial.begin(115200);
  // 初始化VFD
  pinMode(EN, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(DIN, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(RESET, OUTPUT);
  digitalWrite(EN, HIGH);
  delayMicroseconds(100);
  digitalWrite(RESET, LOW);
  delayMicroseconds(5);
  digitalWrite(RESET, HIGH);
  
  VFD_init();
  
  delay(1000);
  // 初始化Web
  webControl.setupServer();
  VFD_setBrightness(85); // 和VFD_init隔一段时间设置
  delay(20);  
  frame.freshDisplay();
  // VFD_WriteStr(0, "00:00:00");
  delay(20);  
}

void clock_loop() {
  static uint32_t time_out = 0;
  uint32_t now_mill = millis();

  if(now_mill >= time_out) 
  {  
    time_out = now_mill + 1000;
    int currentHour = hour();
    int currentMinute = minute();
    int currentSecond = second();
    frame.FrameDisplayTime(currentHour, currentMinute, currentSecond);
    delay(100);
  }
}

void timer_loop() {
    static uint32_t time_out = 0;
    uint32_t now_mill = millis();

    if(now_mill >= time_out) 
    {  
        time_out = now_mill + 1000;

        if (countdownSecond > 0) {
            countdownSecond--;
        } else {
            if (countdownMinute > 0) {
                countdownMinute--;
                countdownSecond = 59;
            } else {
                if (countdownHour > 0) {
                    countdownHour--;
                    countdownMinute = 59;
                    countdownSecond = 59;
                }
            }
        }

        frame.FrameDisplayTime(countdownHour, countdownMinute, countdownSecond);
    }  
}

void loop() {
  // display time 
  bool cur_enable_display = !WebControl::hasSetTime || frame.display_time(displayPeriods, 3);
  if (!enable_display && cur_enable_display) {
    enable_display = cur_enable_display;
    frame.enableDisplay(true);
  }
  if (enable_display && !cur_enable_display) {
    enable_display = cur_enable_display;
    frame.enableDisplay(false);
  }
  timer_loop();
  webControl.processRequests();    
}
