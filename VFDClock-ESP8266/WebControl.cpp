#include "WebControl.h"
#include "Manager.h"

bool WebControl::hasSetTime = false;

/**
  rawliteral用来方便地定义和嵌入包含HTML和JavaScript代码的长字符串，而不需要对其中的特殊字符进行转义。
  HTML压缩解析：https://www.sojson.com/jshtml.html
  JS 压缩解析：https://www.uutils.com/format/js.htm
*/
void WebControl::initLoginPage(String token) {
  loginPage = String(R"rawliteral(
<html><head><title>Login</title><meta charset="UTF-8"><meta name="viewport"content="width=device-width, initial-scale=1"><style>body{font-family:Arial,sans-serif;text-align:center;padding:50px;margin:0}h1{font-size:2.0rem;margin-bottom:40px}input[type=password]{width:100%;padding:10px;font-size:1.2rem;margin:10px 0}button{width:100%;padding:20px;font-size:1.5rem;margin:10px 0}#error{color:red;font-size:1.2rem;margin-top:20px}</style><script>function sendLoginRequest(){var password=document.getElementById('password').value;var token=document.getElementById('token').value;var xhttp=new XMLHttpRequest();xhttp.onreadystatechange=function(){if(this.readyState==4&&this.status==200){if(this.responseText==='success'){window.location.href='/main?token='+token}else{document.getElementById('error').innerText='Incorrect password 🚀'+token}}};xhttp.open('POST','/login',true);xhttp.setRequestHeader('Content-type','application/x-www-form-urlencoded');xhttp.send('password='+password+'&token='+token)}</script></head>
<body><h1>Login...🚀</h1>
<p><input type="password" id="password" placeholder="Enter password"></p>
<input type="hidden" id="token" name="token" value=")rawliteral" + String(token) + R"rawliteral(">
<p><button onclick="sendLoginRequest()">Login</button></p>
<p id="error"></p></body></html>
)rawliteral");  
}

void WebControl::initMainPage() {
  mainPage = String(R"rawliteral(
<html><head><title>Main</title><meta charset="UTF-8"><meta name="viewport"content="width=device-width, initial-scale=1"><style>body{font-family:Arial,sans-serif;text-align:center;padding:50px;margin:0}h1{font-size:2.0rem;margin-bottom:40px}button{width:100%;padding:20px;font-size:1.5rem;margin:10px 0}input[type=range]{width:100%;margin:20px 0}</style><script>function sendRequest(path){var xhttp=new XMLHttpRequest();xhttp.onreadystatechange=function(){if(this.readyState==4&&this.status==200){document.getElementById('status').innerHTML=this.responseText}};xhttp.open('GET',path,true);xhttp.send()}function updateBrightness(val){document.getElementById('brightness').innerHTML=val;sendRequest('/brightness?value='+val)}function sendTime(){var now=new Date();var timeString=now.toTimeString().split(' ')[0];sendRequest('/time?value='+encodeURIComponent(timeString))}function Timer(){window.location.href='/timer'}function toggleFont(){sendRequest('/toggleFont');var button=document.getElementById('fontButton');if(button.getAttribute('data-font-state')==='bold'){button.setAttribute('data-font-state','regular');button.innerHTML='Set Bold Font'}else{button.setAttribute('data-font-state','bold');button.innerHTML='Set Regular Font'}}</script></head>
<body><h1>VFD Control</h1>
<p><button onclick="sendRequest('/on')">Turn On</button></p>
<p><button onclick="sendRequest('/off')">Turn Off</button></p>
<p>Brightness: <span id="brightness">)rawliteral" + String(brightness) + R"rawliteral(</span><input type="range" min="0" max="255" value=")rawliteral" + String(brightness) + R"rawliteral(" onchange="updateBrightness(this.value)"></p>
<p><button onclick="sendTime()">Set Current Time</button></p>
<p><button id="fontButton" data-font-state=")rawliteral" + (isBold ? "bold" : "regular") + R"rawliteral(" onclick="toggleFont()">)rawliteral" + (isBold ? "Set Regular Font" : "Set Bold Font") + R"rawliteral(</button></p>
<p><button onclick="Timer()">Timer</button></p>
<p id="status"></p></body></html>
)rawliteral");
}

void WebControl::handleLEDOn() {
  server.send(200, "text/plain", "ON");
  if (stateMachine) {
//      frameDisplay
      stateMachine->enableDisplay(true);
  }
}

void WebControl::handleLEDOff() {
  server.send(200, "text/plain", "OFF");
  if (stateMachine) {
      stateMachine->enableDisplay(false);
  }
}

void WebControl::handleBrightness() {
  String brightnessValue = server.arg("value");
  brightness = brightnessValue.toInt();
  if (stateMachine) {
      stateMachine->setBrightness(brightness);
  }
  server.send(200, "text/plain", "brightness set to " + brightnessValue);
}

void WebControl::handleToggleFont() {
  isBold = !isBold;
  server.send(200, "text/plain", isBold ? "Bold font set" : "Regular font set");
  if (stateMachine) {
      stateMachine->setFont(isBold);
  }
  
}

void WebControl::handleTime() {
  String time = server.arg("value");
  hasSetTime = true;
  // Parse the time string and set the ESP8266 time
  int hour = time.substring(0, 2).toInt();
  int minute = time.substring(3, 5).toInt();
  int second = time.substring(6, 8).toInt();
  setTime(hour, minute, second, day(), month(), year()); // Assumes date is unchanged
  
  server.send(200, "text/plain", "Current time is " + time);
}

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

void WebControl::handleTimerPage() {
  this->initTimerPage();
  server.send_P(200, "text/html", timerPage.c_str());
}


void WebControl::handleLogin() {
  if (server.method() == HTTP_POST) {
    String password = server.arg("password");
    String token = server.arg("token");
    if (userTokens.find(token) != userTokens.end() && password == correctPassword) {
      userTokens[token] = true; // 更新为已登录状态
      server.send(200, "text/plain", "success");
    } else {
      server.send(200, "text/plain", "failure");
    }
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
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
  server.on("/toggleFont", [this]() { handleToggleFont(); });
  server.onNotFound([this]() { handleNotFound(); });

  // 启动Web服务器
  server.begin();
}
