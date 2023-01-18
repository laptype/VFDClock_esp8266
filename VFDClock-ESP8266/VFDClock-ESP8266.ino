#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>  
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>               // NTPClient库文件
#include <TimeLib.h>                 // Arduino时间库

#define NTP_OFFSET 8 * 3600 // + 8h，北京时间
#define NTP_INTERVAL 8* 60 * 60 * 1000 // 12 * 60 * 60 * 1000 12小时更新
#define NTP_ADDRESS  "cn.ntp.org.cn"

unsigned char number[10][5] = {
   0x3E, 0x51, 0x49, 0x45, 0x3E,// 0
   0x00, 0x42, 0x7F, 0x40, 0x00,// 1
   0x42, 0x61, 0x51, 0x49, 0x46,// 2
   0x21, 0x41, 0x45, 0x4B, 0x31,// 3
   0x18, 0x14, 0x12, 0x7F, 0x10,// 4
   0x27, 0x45, 0x45, 0x45, 0x39,// 5
   0x3C, 0x4A, 0x49, 0x49, 0x30,// 6
   0x01, 0x71, 0x09, 0x05, 0x03,// 7
   0x36, 0x49, 0x49, 0x49, 0x36,// 8
   0x06, 0x49, 0x49, 0x29, 0x1E,// 9
};
unsigned char dispalyTemp[6][5] = {0};
int last_number[6] = {0,0,0,0,0,0};
int now_number[6] = {6,6,6,6,6,6};

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS,NTP_OFFSET,NTP_INTERVAL);
// 'cn.ntp.org.cn' is used (default server) with +1 hour offset (3600 seconds) 60 seconds (60000 milliseconds) update interval


//适用于esp8266的引脚
uint8_t din   = 12; // DA
uint8_t clk   = 14; // CK
uint8_t cs    = 4; // CS
uint8_t Reset = 1; // RS
uint8_t en    = 3; // EN 0

char *str_time = "00:00:00";
String format_time = "00:00:00";


//vfd时钟驱动部分
void write_6302(unsigned char w_data)
{
  unsigned char i;
  for (i = 0; i < 8; i++)
  {
    digitalWrite(clk, LOW);
    if ( (w_data & 0x01) == 0x01)
    {
      digitalWrite(din, HIGH);
    }
    else
    {
      digitalWrite(din, LOW);
    }
    w_data >>= 1;
    digitalWrite(clk, HIGH);
  }
}

void VFD_cmd(unsigned char command)
{
  digitalWrite(cs, LOW);
  write_6302(command);
  digitalWrite(cs, HIGH);
  delayMicroseconds(5);
}

void S1201_show(void)
{
  digitalWrite(cs, LOW);//开始传输
  write_6302(0xe8);     //地址寄存器起始位置
  digitalWrite(cs, HIGH); //停止传输

}

void VFD_init()
{
  //SET HOW MANY digtal numbers
  digitalWrite(cs, LOW);
  write_6302(0xe0);
  delayMicroseconds(5);
  write_6302(0x07);//8 digtal
  digitalWrite(cs, HIGH);
  delayMicroseconds(5);

  //set bright
  digitalWrite(cs, LOW);
  write_6302(0xe4);
  delayMicroseconds(5);
  
  //write_6302(0x33);//esp32用这个
  write_6302(0xff);//8266用这个
  digitalWrite(cs, HIGH);
  delayMicroseconds(5);

}

/******************************
  在指定位置打印一个字符(用户自定义,所有CG-ROM中的)
  x:0~11;chr:要显示的字符编码
*******************************/
void S1201_WriteOneChar(unsigned char x, unsigned char chr)
{
  digitalWrite(cs, LOW);  //开始传输
  write_6302(0x20 + x); //地址寄存器起始位置
  write_6302(chr + 0x30);
  digitalWrite(cs, HIGH); //停止传输
  S1201_show();

}
/******************************
  在指定位置打印字符串
  (仅适用于英文,标点,数字)
  x:0~11;str:要显示的字符串
*******************************/
void S1201_WriteStr(unsigned char x, char *str)
{
  digitalWrite(cs, LOW);  //开始传输
  write_6302(0x20 + x); //地址寄存器起始位置
  while (*str)
  {
    write_6302(*str); //ascii与对应字符表转换
    str++;
  }
  digitalWrite(cs, HIGH); //停止传输
  S1201_show();

}
/******************************
  在指定位置打印自定义字符
  
  x:位置，有ROOM位置;*s:要显示的字符字模
*******************************/
void VFD_WriteUserFont(unsigned char x, unsigned char y,unsigned char *s)
{
  unsigned char i=0;
  unsigned char ii=0;
  digitalWrite(cs, LOW); //开始传输 
  write_6302(0x40+y);//地址寄存器起始位置
  for(i=0;i<7;i++)
    write_6302(s[i]);
  digitalWrite(cs, HIGH); //停止传输

  digitalWrite(cs, LOW);
  write_6302(0x20+x);
  write_6302(0x00+y);   
  digitalWrite(cs, HIGH);

  S1201_show();
}

//vfd驱动部分结束

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


  timeClient.begin();
    // 建立WiFiManager对象
}

void up_now_num(String format_time)
{
  unsigned char i;
  now_number[0] = format_time[0]-'0';
  now_number[1] = format_time[1]-'0';
  now_number[2] = format_time[3]-'0';
  now_number[3] = format_time[4]-'0';
  now_number[4] = format_time[6]-'0';
  now_number[5] = format_time[7]-'0';
}
void up_last_num(){
  last_number[0] = now_number[0];
  last_number[1] = now_number[1];
  last_number[2] = now_number[2];
  last_number[3] = now_number[3];
  last_number[4] = now_number[4];
  last_number[5] = now_number[5];
}

void FrameRefresh(unsigned char num, unsigned char mCnt1, unsigned char mCnt2, unsigned char x)
{
  dispalyTemp[num][0] = (number[last_number[num]][0] << mCnt1) | (number[now_number[num]][0] >> mCnt2);
  dispalyTemp[num][1] = (number[last_number[num]][1] << mCnt1) | (number[now_number[num]][1] >> mCnt2);  
  dispalyTemp[num][2] = (number[last_number[num]][2] << mCnt1) | (number[now_number[num]][2] >> mCnt2);
  dispalyTemp[num][3] = (number[last_number[num]][3] << mCnt1) | (number[now_number[num]][3] >> mCnt2);
  dispalyTemp[num][4] = (number[last_number[num]][4] << mCnt1) | (number[now_number[num]][4] >> mCnt2);
  dispalyTemp[num][5] = (number[last_number[num]][5] << mCnt1) | (number[now_number[num]][5] >> mCnt2);
  VFD_WriteUserFont(x,x,dispalyTemp[num]);
}

void SecDispalyRefresh() {
  unsigned char i;
  for(i=1;i<7;i++) {

    if (now_number[0] != last_number[0]) FrameRefresh(0,i,7-i,0);
    if (now_number[1] != last_number[1]) FrameRefresh(1,i,7-i,1);
    if (now_number[2] != last_number[2]) FrameRefresh(2,i,7-i,3);
    if (now_number[3] != last_number[3]) FrameRefresh(3,i,7-i,4);
    if (now_number[4] != last_number[4]) FrameRefresh(4,i,7-i,6);
    if (now_number[5] != last_number[5]) FrameRefresh(5,i,7-i,7);

    delay(20);
  }
}

void clock_loop() {
  static uint32_t time_out = 0; // 刷新时间控制
  uint32_t now_mill = millis(); // 当前的系统时间
  
  // 如果系统计时器溢出
  if(now_mill >= time_out) 
  {
    // 设定下次刷新的时间点（1s以后再次刷新）
    time_out = now_mill + 1000;
    // 更新TNP时间
    timeClient.update();

    // 显示时间
    format_time = timeClient.getFormattedTime();
    up_now_num(format_time);
    SecDispalyRefresh();
    up_last_num();
    char *str_time = &format_time[0]; 
    S1201_WriteStr(0, str_time);
    delay(870);
  }
}

void loop()
{
 clock_loop();
}
