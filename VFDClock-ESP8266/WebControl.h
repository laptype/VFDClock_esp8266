#ifndef WEBCONTROL_H
#define WEBCONTROL_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <TimeLib.h>
#include <map> // 使用C++标准库的map进行token管理
#include "FrameRefresh.h"

class WebControl {
public:
    WebControl(FrameRefresh* frameRefresh) : apIP(192, 168, 4, 1), server(80), correctPassword("123"), frameRefresh(frameRefresh) {}
    
    void setupServer();

    void processRequests();

    static bool hasSetTime;
    int brightness = 85; // Default brightness value
    bool isBold = false;
private:
    const byte DNS_PORT = 53;
    IPAddress apIP;
    DNSServer dnsServer;
    ESP8266WebServer server;
    const char* correctPassword;
    bool isLoggedIn;  // 用于会话管理

    FrameRefresh* frameRefresh; // Pointer to FrameRefresh
    std::map<String, bool> userTokens; // 存储token及其登录状态
    
    void handleClient();
    void checkWiFiAndSleep();
    void processNextRequest();
    void enterLowPowerMode();
    void exitLowPowerMode();
    void handleLoginPage();
    void handleLogin();
    void handleMainPage();
    void handleLEDOn();
    void handleLEDOff();
    void handleBrightness();
    void handleTime();
    void handleToggleFont();
    void handleNotFound();
    void initMainPage();
    void initLoginPage(String token);

    String loginPage;
    String mainPage; // mainPage作为String来处理
};

#endif
