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

    static const char loginPage[];
    static const char mainPage[];
};

#endif
