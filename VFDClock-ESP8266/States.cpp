#include "States.h"
#include "StateMachine.h"

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


