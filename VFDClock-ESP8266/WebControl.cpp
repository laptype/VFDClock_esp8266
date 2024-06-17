#include "WebControl.h"
#include "VFDdriver.h"

bool WebControl::hasSetTime = false;

/**
  rawliteral用来方便地定义和嵌入包含HTML和JavaScript代码的长字符串，而不需要对其中的特殊字符进行转义。
*/
const char WebControl::loginPage[] PROGMEM = R"rawliteral(
<html>
<head>
<title>Login</title>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
body { font-family: Arial, sans-serif; text-align: center; padding: 50px; margin: 0; }
h1 { font-size: 2.0rem; margin-bottom: 40px; }
input[type=password] { width: 100%; padding: 10px; font-size: 1.2rem; margin: 10px 0; }
button { width: 100%; padding: 20px; font-size: 1.5rem; margin: 10px 0; }
#error { color: red; font-size: 1.2rem; margin-top: 20px; }
</style>
<script>
function sendLoginRequest() {
  var password = document.getElementById('password').value;
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      if (this.responseText === 'success') {
        window.location.href = '/main';
      } else {
        document.getElementById('error').innerText = 'Incorrect password 🚀';
      }
    }
  };
  xhttp.open('POST', '/login', true);
  xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
  xhttp.send('password=' + encodeURIComponent(password));
}
</script>
</head>
<body>
<h1>Login...🚀</h1>
<p><input type="password" id="password" placeholder="Enter password"></p>
<p><button onclick="sendLoginRequest()">Login</button></p>
<p id="error"></p>
</body>
</html>
)rawliteral";

void WebControl::initMainPage() {
  mainPage = String(R"rawliteral(
<html>
<head>
<title>Main</title>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
body { font-family: Arial, sans-serif; text-align: center; padding: 50px; margin: 0; }
h1 { font-size: 2.0rem; margin-bottom: 40px; }
button { width: 100%; padding: 20px; font-size: 1.5rem; margin: 10px 0; }
input[type=range] { width: 100%; margin: 20px 0; }
</style>
<script>
function sendRequest(path) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById('status').innerHTML = this.responseText;
    }
  };
  xhttp.open('GET', path, true);
  xhttp.send();
}
function updateBrightness(val) {
  sendRequest('/brightness?value=' + val);
}
function sendTime() {
  var now = new Date();
  var timeString = now.toTimeString().split(' ')[0];
  sendRequest('/time?value=' + encodeURIComponent(timeString));
}
</script>
</head>
<body>
<h1>VFD Control</h1>
<p><button onclick="sendRequest('/on')">Turn On</button></p>
<p><button onclick="sendRequest('/off')">Turn Off</button></p>
<p>Brightness: <input type="range" min="0" max="255" value=")rawliteral" + String(brightness) + R"rawliteral(" onchange="updateBrightness(this.value)"></p>
<p><button onclick="sendTime()">Set Current Time</button></p>
<p id="status">OFF</p>
</body>
</html>
)rawliteral");
}



void WebControl::handleLEDOn() {
  server.send(200, "text/plain", "ON");
  VFD_enable(true);
}

void WebControl::handleLEDOff() {
  server.send(200, "text/plain", "OFF");
  VFD_enable(false);
}

void WebControl::handleBrightness() {
  String brightnessValue = server.arg("value");
  brightness = brightnessValue.toInt();
  VFD_setBrightness(brightness);
  server.send(200, "text/plain", "brightness set to " + brightnessValue);
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
  server.send_P(200, "text/html", loginPage);
}

void WebControl::handleMainPage() {
  if (isLoggedIn) {
    // 初始化HTML页面
    this->initMainPage();
    server.send_P(200, "text/html", mainPage.c_str());
  } else {
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", ""); // 重定向到登录页面
  }
}

void WebControl::handleLogin() {
  if (server.method() == HTTP_POST) {
    String password = server.arg("password");
    if (password == correctPassword) {
      isLoggedIn = true;
      server.send(200, "text/plain", "success");
    } else {
      isLoggedIn = false;
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
    enterLowPowerMode();
  } else {
    exitLowPowerMode();
  }
}

void WebControl::handleClient() {
  server.handleClient();
}

void WebControl::processNextRequest() {
  dnsServer.processNextRequest();
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
  server.on("/on", [this]() { handleLEDOn(); });
  server.on("/off", [this]() { handleLEDOff(); });
  server.on("/brightness", [this]() { handleBrightness(); });
  server.on("/time", [this]() { handleTime(); });
  server.onNotFound([this]() { handleNotFound(); });

  // 启动Web服务器
  server.begin();
}
