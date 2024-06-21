//
// Created by 12053 on 2024/6/20.
//

#include "State.h"

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