#include "WebControl.h"
#include "States.h"

bool WebControl::hasSetTime = false;

void WebControl::handleLoginPage() {
  String token = server.client().remoteIP().toString();
  if (userTokens.find(token) != userTokens.end() && userTokens[token]) {
    this->initMainPage();
    server.send_P(200, "text/html", mainPage.c_str());
  } else {
    userTokens[token] = false; // 初始化为未登录状态
    this->initLoginPage(token);
    server.send_P(200, "text/html", loginPage.c_str());    
  }
}

void WebControl::handleMainPage() {
  String token = server.arg("token");
  if (userTokens.find(token) != userTokens.end() && userTokens[token]) {
    // 初始化HTML页面
    this->initMainPage();
    server.send_P(200, "text/html", mainPage.c_str());
  } else {
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", ""); // 重定向到登录页面
  }
}



void WebControl::handleNotFound() {
  server.sendHeader("Location", String("http://") + apIP.toString(), true);
  server.send(302, "text/plain", "");
}

void WebControl::handleClient() {
  server.handleClient();
}

void WebControl::processNextRequest() {
  dnsServer.processNextRequest();
}

void WebControl::enterLowPowerMode() {
  WiFi.setSleepMode(WIFI_LIGHT_SLEEP); // 设置WiFi为浅睡眠模式
  delay(1);                            // 确保设置生效
}

void WebControl::exitLowPowerMode() {
  WiFi.setSleepMode(WIFI_NONE_SLEEP); // 取消睡眠模式
  delay(1);                           // 确保设置生效
}

void WebControl::checkWiFiAndSleep() {
  if (WiFi.softAPgetStationNum() == 0) {
    // clearUnloggedTokens();
    userTokens.clear();  // 清空 userTokens map
    preTimeStamp = 0;    // 重置
    enterLowPowerMode();
  } else {
    exitLowPowerMode();
  }
}

void WebControl::processRequests() {
  // 仅在没有进入浅睡眠模式时处理请求
  if (WiFi.getSleepMode() != WIFI_LIGHT_SLEEP) {
    processNextRequest(); // 处理重定向请求
    handleClient();
  }
  delay(20);
  checkWiFiAndSleep();
}

void WebControl::setupServer() {

  // 设置WiFi AP（不加密码）
  const char* ssid = "Loading...🚀";
  WiFi.softAP(ssid); // 不加密码
  delay(100);

  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  dnsServer.start(DNS_PORT, "*", apIP);

  // 定义Web服务器路由
  server.on("/", [this]() { handleLoginPage(); });
  server.on("/login", HTTP_POST, [this]() { handleLogin(); });
  server.on("/main", [this]() { handleMainPage(); });
  server.on("/timer", [this]() { handleTimerPage(); });  
  server.on("/on", [this]() { handleLEDOn(); });
  server.on("/off", [this]() { handleLEDOff(); });
  server.on("/brightness", [this]() { handleBrightness(); });
  server.on("/time", [this]() { handleTime(); });
  server.on("/timerSet", [this]() { handleTimeSet(); });
  server.on("/toggleFont", [this]() { handleToggleFont(); });
  server.onNotFound([this]() { handleNotFound(); });

  // 启动Web服务器
  server.begin();
}
