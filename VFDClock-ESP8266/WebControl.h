#ifndef WEBCONTROL_H
#define WEBCONTROL_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <TimeLib.h>

class WebControl {
public:
    WebControl() : apIP(192, 168, 4, 1), server(80), correctPassword("123") {}
    
    void setupServer();

    void processRequests();

    static bool hasSetTime;
    int brightness = 100; // Default brightness value

private:
    const byte DNS_PORT = 53;
    IPAddress apIP;
    DNSServer dnsServer;
    ESP8266WebServer server;
    const char* correctPassword;
    bool isLoggedIn;  // 用于会话管理

    
    
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
    void handleNotFound();
    void initMainPage();

    static const char loginPage[];
    String mainPage; // mainPage作为String来处理
};

#endif
