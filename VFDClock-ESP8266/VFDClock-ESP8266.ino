#include "Manager.h"
#include <TimeLib.h> // Include the Time library for time functions
// #include <ESP8266WebServer.h>
// ESP8266WebServer server(80);



StateMachine machine = StateMachine();

bool enable_display = true;
//FrameRefresh frame = FrameRefresh();
//WebControl webControl = WebControl(&frame); // 创建WebControl对象



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

  machine.init();
}

void loop() {
//  // display time
//  bool cur_enable_display = !WebControl::hasSetTime || frame.isDisplay(displayPeriods, 3);
//  if (!enable_display && cur_enable_display) {
//    enable_display = cur_enable_display;
//    frame.enableDisplay(true);
//  }
//  if (enable_display && !cur_enable_display) {
//    enable_display = cur_enable_display;
//    frame.enableDisplay(false);
//  }
//  clock_loop();
//  webControl.processRequests();
    machine.running();
}
