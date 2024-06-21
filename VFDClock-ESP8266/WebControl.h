#ifndef WEBCONTROL_H
#define WEBCONTROL_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <TimeLib.h>
#include <map> // 使用C++标准库的map进行token管理

class StateMachine; // Forward declaration
class WebControl {
public:
    WebControl(StateMachine* stateMachine) : apIP(192, 168, 4, 1), server(80), correctPassword("123"), stateMachine(stateMachine) {}
    
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

    StateMachine* stateMachine; // Pointer to stateMachine
    std::map<String, bool> userTokens; // 存储token及其登录状态
    bool isTimerRunning = false;
    int TimerMinute = 0;
    int preTimeStamp = 0;
    /**
     * main page
     */
    void handleLEDOn();
    void handleLEDOff();
    void handleBrightness();
    void handleTime();
    void handleToggleFont();
    /**
     * login page
     */
    void handleLogin();

    void handleTimeSet();

    void handleClient();

    void checkWiFiAndSleep();
    void processNextRequest();
    void enterLowPowerMode();
    void exitLowPowerMode();

    void handleLoginPage();
    void handleMainPage();
    void handleTimerPage();

    void handleNotFound();

    void initMainPage();
    void initLoginPage(String token);
    void initTimerPage();

    String loginPage;
    String mainPage; // mainPage作为String来处理
    String timerPage;
};

#endif
