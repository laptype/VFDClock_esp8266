//
// Created by 12053 on 2024/6/20.
//

#include "StateMachine.h"

StateMachine::StateMachine() {
    frame = new FrameRefresh();
    webControl = new WebControl(this); // 创建WebControl对象
    stateList[DISPLAY_STATE] = new DisplayTimeState();
    stateList[TIMER_STATE] = new TimerState();
    stateList[LOW_POWER_STATE] = new LowPowerState();
}

void StateMachine::init() {
    VFD_init();
    delay(1000);
    webControl->setupServer();
    frame->setBrightness(85);
    delay(20);
    frame->freshDisplay();
    delay(20);

    setState(DISPLAY_STATE);
}

StateMachine::~StateMachine() {
    for (int i = 0; i < 3; ++i) {
        delete stateList[i];
    }
    delete frame;
    delete webControl;
}

void StateMachine::displayTime(int currentHour, int currentMinute, int currentSecond) {
    frame->displayFrameTime(currentHour, currentMinute, currentSecond);
}

void StateMachine::running() {
    if (curState) {
        curState->handle(*this);
    }
    webControl->processRequests();
}

void StateMachine::setState(int stateId) {
    curState = stateList[stateId];
}

void StateMachine::enableDisplay(bool enable) {
    frame->enableDisplay(enable);
}

void StateMachine::setBrightness(int brightness) {
    frame->setBrightness(brightness);
}

void StateMachine::setFont(bool bold) {
    frame->setFont(bold);
}
