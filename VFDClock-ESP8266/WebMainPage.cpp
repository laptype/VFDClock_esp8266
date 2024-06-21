#include "WebControl.h"
#include "State.h"

void WebControl::initMainPage() {
    mainPage = String(R"rawliteral(
<html><head><title>Main</title><meta charset="UTF-8"><meta name="viewport"content="width=device-width, initial-scale=1"><style>body{font-family:Arial,sans-serif;text-align:center;padding:50px;margin:0}h1{font-size:2.0rem;margin-bottom:40px}button{width:100%;padding:20px;font-size:1.5rem;margin:10px 0}input[type=range]{width:100%;margin:20px 0}</style><script>function sendRequest(path){var xhttp=new XMLHttpRequest();xhttp.onreadystatechange=function(){if(this.readyState==4&&this.status==200){document.getElementById('status').innerHTML=this.responseText}};xhttp.open('GET',path,true);xhttp.send()}function updateBrightness(val){document.getElementById('brightness').innerHTML=val;sendRequest('/brightness?value='+val)}function sendTime(){var now=new Date();var timeString=now.toTimeString().split(' ')[0];sendRequest('/time?value='+encodeURIComponent(timeString))}function Timer(){window.location.href='/timer'}function toggleFont(){sendRequest('/toggleFont');var button=document.getElementById('fontButton');if(button.getAttribute('data-font-state')==='bold'){button.setAttribute('data-font-state','regular');button.innerHTML='Set Bold Font'}else{button.setAttribute('data-font-state','bold');button.innerHTML='Set Regular Font'}}</script></head>
<body><h1>VFD Control</h1><p><button onclick="sendRequest('/on')">Turn On</button></p>
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
    stateMachine->setState(DISPLAY_STATE);
}