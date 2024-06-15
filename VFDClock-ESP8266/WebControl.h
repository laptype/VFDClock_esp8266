#ifndef WEBCONTROL_H
#define WEBCONTROL_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

#include <TimeLib.h> // Include the Time library for time functions

namespace WebControl {
  // 使用extern关键字声明服务器对象
  extern DNSServer dnsServer;
  extern ESP8266WebServer server;
  extern bool hasSetTime;
  // 函数声明
  void handleRoot();
  void handleLEDOn();
  void handleLEDOff();
  void handleBrightness();
  void handleTime();
  void setupServer();
}

#endif // WEBCONTROL_H
