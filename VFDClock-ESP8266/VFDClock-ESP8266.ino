#include "State.h"
#include <TimeLib.h> // Include the Time library for time functions
// #include <ESP8266WebServer.h>
// ESP8266WebServer server(80);


StateMachine machine = StateMachine();

void setup() {

  // Serial.begin(115200);
  // 初始化VFD
  pinMode(EN, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(DIN, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(RESET, OUTPUT);
  digitalWrite(EN, HIGH);
  delayMicroseconds(100);
  digitalWrite(RESET, LOW);
  delayMicroseconds(5);
  digitalWrite(RESET, HIGH);

  machine.init();
}

void loop() {
    machine.running();
}
