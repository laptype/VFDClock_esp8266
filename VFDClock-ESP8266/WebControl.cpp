#include "WebControl.h"
#include "VFDdriver.h"

namespace WebControl {
  const byte DNS_PORT = 53;
  IPAddress apIP(192, 168, 4, 1);
  DNSServer dnsServer;
  ESP8266WebServer server(80);  // 定义服务器对象
  bool hasSetTime = false;
  const char* correctPassword = "123"; // 设置正确的密码
  // HTML内容
  const char loginPage[] PROGMEM = R"rawliteral(
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

  const char mainPage[] PROGMEM = R"rawliteral(
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
  <p>Brightness: <input type="range" min="0" max="255" value="128" onchange="updateBrightness(this.value)"></p>
  <p><button onclick="sendTime()">Set Current Time</button></p>
  <p id="status">OFF</p>
  </body>
  </html>
  )rawliteral";


  void handleLEDOn() {
    server.send(200, "text/plain", "ON");
    VFD_enable(true);
  }

  void handleLEDOff() {
    server.send(200, "text/plain", "OFF");
    VFD_enable(false);
  }

  void handleBrightness() {
    String brightnessValue = server.arg("value");
    int brightness = brightnessValue.toInt();
    VFD_setBrightness(brightness);
    server.send(200, "text/plain", "brightness set to " + brightnessValue);
  }

  void handleTime() {
    String time = server.arg("value");
    hasSetTime = true;
    // Parse the time string and set the ESP8266 time
    int hour = time.substring(0, 2).toInt();
    int minute = time.substring(3, 5).toInt();
    int second = time.substring(6, 8).toInt();
    setTime(hour, minute, second, day(), month(), year()); // Assumes date is unchanged

    server.send(200, "text/plain", "Current time is " + time);
  }
  // 处理登录页面请求
  void handleLoginPage() {
    server.send_P(200, "text/html", loginPage);
  }

  // 处理主页面请求
  void handleMainPage() {
    server.send_P(200, "text/html", mainPage);
  }

  // 处理登录请求
  void handleLogin() {
    if (server.method() == HTTP_POST) {
      String password = server.arg("password");
      if (password == correctPassword) {
        server.send(200, "text/plain", "success");
      } else {
        server.send(200, "text/plain", "failure");
      }
    } else {
      server.send(405, "text/plain", "Method Not Allowed");
    }
  }
  // 处理所有其他路径的请求，进行重定向
  void handleNotFound() {
    server.sendHeader("Location", String("http://") + apIP.toString(), true);
    server.send(302, "text/plain", "");
  }

  void setupServer() {
    // 设置WiFi AP名称和密码
    const char* ssid = "Loading...🚀";
    // const char* password = "12345678";
    // WiFi.softAP(ssid, password);
    // 设置WiFi AP
    WiFi.softAP(ssid); // 不加密码
    delay(100);

    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

    dnsServer.start(DNS_PORT, "*", apIP);

    // 定义Web服务器路由
    server.on("/", handleLoginPage);
    server.on("/login", HTTP_POST, handleLogin);
    server.on("/main", handleMainPage);
    server.on("/on", handleLEDOn);
    server.on("/off", handleLEDOff);
    server.on("/brightness", handleBrightness);
    server.on("/time", handleTime);
    server.onNotFound(handleNotFound); // 处理所有其他路径的请求, 强制重定向到 /login


    // 启动Web服务器
    server.begin();
  }
} // namespace WebControl
