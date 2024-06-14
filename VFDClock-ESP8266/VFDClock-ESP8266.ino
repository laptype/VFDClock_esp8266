#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>  
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>               // NTPClient库文件
#include <TimeLib.h>                 // Arduino时间库
#include "VFDdriver.h"
#include "frameRefresh.h"

#define NTP_OFFSET 8 * 3600 // + 8h，北京时间
#define NTP_INTERVAL 8* 60 * 60 * 1000 // 12 * 60 * 60 * 1000 12小时更新
#define NTP_ADDRESS  "cn.ntp.org.cn"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS,NTP_OFFSET,NTP_INTERVAL);
// 'cn.ntp.org.cn' is used (default server) with +1 hour offset (3600 seconds) 60 seconds (60000 milliseconds) update interval

char *str_time = "00:00:00";
String format_time = "00:00:00";

void start_display()
{
  S1201_WriteStr(0, "00000000");
  delay(200);
  unsigned char i;
  for(i=1;i<7;i++) {
    FrameRefresh(0,i,7-i,0);
    delay(20);
  }
  S1201_WriteStr(0, "60000000");
  delay(200);
  for(i=1;i<7;i++) {
    FrameRefresh(1,i,7-i,1);
    delay(20);
  }
  S1201_WriteStr(0, "66000000");
  delay(200);
  for(i=1;i<7;i++) {
    FrameRefresh(2,i,7-i,2);
    delay(20);
  }
  S1201_WriteStr(0, "66600000");
  delay(200);
  for(i=1;i<7;i++) {
    FrameRefresh(3,i,7-i,3);
    delay(20);
  }
  S1201_WriteStr(0, "66660000");
  delay(200);
  for(i=1;i<7;i++) {
    FrameRefresh(4,i,7-i,4);
    delay(20);
  }
  S1201_WriteStr(0, "66666000");
  delay(200);
  for(i=1;i<7;i++) {
    FrameRefresh(5,i,7-i,5);
    delay(20);
  }
  S1201_WriteStr(0, "66666600");
  delay(200);
}

void setup(){
  WiFiManager wifiManager;
  Serial.begin(115200);  
  wifiManager.autoConnect("VFDClock");
  // wifiManager.autoConnect("AutoConnectAP", "12345678");

  Serial.println(""); 
  Serial.print("ESP8266 Connected to ");
  Serial.println(WiFi.SSID());              // WiFi名称
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // IP

  //vfd引脚
  pinMode(en, OUTPUT);
  pinMode(clk, OUTPUT);
  pinMode(din, OUTPUT);
  pinMode(cs, OUTPUT);
  pinMode(Reset, OUTPUT);
  digitalWrite(en, HIGH);
  digitalWrite(Reset, LOW);
  delayMicroseconds(5);
  digitalWrite(Reset, HIGH);
  VFD_init(); 
  //vfd引脚结束
  start_display(); // 初始显示界面
  
  timeClient.begin();
  timeClient.update();
  setTime(timeClient.getEpochTime());
}


void clock_loop() {
  static uint32_t time_out = 0;
  uint32_t now_mill = millis();
  
  if(now_mill >= time_out) 
  {
    time_out = now_mill + 1000;
    // 如果连了wifi, 更新时间
    if (WiFi.status() == WL_CONNECTED) {
      timeClient.update();
      setTime(timeClient.getEpochTime());
    }
    
    int currentHour = hour();
    int currentMinute = minute();
    int currentSecond = second();

    // Check if the current time is within the display periods
    bool displayTime = (
      (currentHour >= 10 && currentHour < 12) ||
      (currentHour == 12 && currentMinute == 0) ||
      (currentHour >= 14 && currentHour < 17) ||
      (currentHour == 17 && currentMinute <= 30) ||
      (currentHour >= 18 && currentHour < 23) ||
      (currentHour == 22 && currentMinute <= 30)
    );

    if (displayTime) {
      char timeBuffer[9]; // Buffer to hold the formatted time string "HH:MM:SS"
      sprintf(timeBuffer, "%02d:%02d:%02d", currentHour, currentMinute, currentSecond);
      
      // Print the formatted time to the Serial Monitor
      Serial.println(timeBuffer);
      
      format_time = String(timeBuffer);
      up_now_num(format_time);
      SecDispalyRefresh();
      up_last_num();
      S1201_WriteStr(0, timeBuffer);
    } else {
      // Clear the display if time is not within the display periods
      S1201_WriteStr(0, "        "); // Assuming 8-character wide display, use spaces to clear
    }

    delay(900);
  }
}

void loop()
{
  clock_loop();
}
