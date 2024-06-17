#include "FrameRefresh.h"
#include "WebControl.h"
#include <TimeLib.h> // Include the Time library for time functions
// #include <ESP8266WebServer.h>
// ESP8266WebServer server(80);

FrameRefresh::TimePeriod displayPeriods[] = {
    {10, 0, 12, 0},
    {13, 30, 17, 30},
    {18, 0, 23, 0},
};

bool enable_display = true;
WebControl webControl; // 创建WebControl对象

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
  VFD_setBrightness(128); // 和VFD_init隔一段时间设置
}

void clock_loop() {
  static uint32_t time_out = 0;
  uint32_t now_mill = millis();

  if(now_mill >= time_out) 
  {  
    time_out = now_mill + 1000;
    FrameRefresh::FrameDisplayTime();
    delay(100);
  }
}

void loop() {
  // display time 
  bool cur_enable_display = !WebControl::hasSetTime || FrameRefresh::display_time(displayPeriods, 3);
  if (!enable_display && cur_enable_display) {
    VFD_enable(true);
    enable_display = cur_enable_display;
  }
  if (enable_display && !cur_enable_display) {
    VFD_enable(false);
    enable_display = cur_enable_display;
  }
  clock_loop();
  webControl.processRequests();

}
