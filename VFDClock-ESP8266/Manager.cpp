#include "Manager.h"

void DisplayTimeState::display(StateMachine &stateMachine) {
    uint32_t now_mill = millis();
    if(now_mill >= time_out)
    {
        time_out = now_mill + 1000;
        int currentHour = hour();
        int currentMinute = minute();
        int currentSecond = second();
        stateMachine.displayTime(currentHour, currentMinute, currentSecond);
        delay(100);
    }
}

void DisplayTimeState::handle(StateMachine &stateMachine) {
    bool cur_enable_display = !WebControl::hasSetTime || FrameRefresh::isDisplay(displayPeriods, 3);
    if (!enable_display && cur_enable_display) {
        enable_display = cur_enable_display;
        stateMachine.enableDisplay(true);
    }
    if (enable_display && !cur_enable_display) {
        enable_display = cur_enable_display;
        stateMachine.enableDisplay(false);
    }
    display(stateMachine);
}

void DisplayTimeState::stateInit() {
    time_out = 0;
}

void TimerState::display(StateMachine &stateMachine) {
    uint32_t now_mill = millis();

    if(now_mill >= time_out)
    {
        time_out = now_mill + 1000;

        if (countdownSecond > 0) {
            countdownSecond--;
        } else {
            if (countdownMinute > 0) {
                countdownMinute--;
                countdownSecond = 59;
            } else {
                if (countdownHour > 0) {
                    countdownHour--;
                    countdownMinute = 59;
                    countdownSecond = 59;
                }
            }
        }

        stateMachine.displayTime(countdownHour, countdownMinute, countdownSecond);
    }
}

void TimerState::handle(StateMachine &stateMachine) {
    display(stateMachine);
}

void TimerState::stateInit() {
    time_out = 0;
}

void LowPowerState::handle(StateMachine &stateMachine) {

}

void LowPowerState::stateInit() {

}

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



