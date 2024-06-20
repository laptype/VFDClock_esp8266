#ifndef VFDCLOCK_ESP8266_STATEMACHINE_H
#define VFDCLOCK_ESP8266_STATEMACHINE_H
#include "FrameRefresh.h"
#include "WebControl.h"
#include "States.h"

class StateMachine {
private:
    FrameRefresh* frame;
    WebControl* webControl;
    State* stateList[3] = {nullptr};
    State* curState = nullptr;

public:
    StateMachine();
    ~StateMachine();
    void init();
    void displayTime(int currentHour, int currentMinute, int currentSecond);
    void setState(int stateId);
    void running();
    void enableDisplay(bool en);
    void setBrightness(int brightness);
    void setFont(bool bold);
};

#endif //VFDCLOCK_ESP8266_STATEMACHINE_H
